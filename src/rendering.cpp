#include "rendering.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "camera.h"
#include "common.h"
#include "spriteLibrary.h"


void RenderSprite_World(SpriteRenderInfo spriteRenderInfo, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale, float alpha, bool flipped){
  int frame = spriteRenderInfo.frame;
  Sprite* sprite = spriteRenderInfo.sprite;
  SDL_FRect tilesetRect;
  if(GetSpriteCount(sprite) > 1){
    int width = sprite->width / sprite->sprite_count_x;
    int height = sprite->height / sprite->sprite_count_y;
    tilesetRect.w = width;
    tilesetRect.h = height;
    tilesetRect.x = (frame % sprite->sprite_count_x) * width;
    tilesetRect.y = (frame / sprite->sprite_count_x) * height;
  }
  else{
    tilesetRect.w = sprite->width;
    tilesetRect.h = sprite->height;
    tilesetRect.x = 0;
    tilesetRect.y = 0; 
  }
  SDL_FRect rect;
  rect.x = x;
  rect.y = y;
  float final_scale = UPSCALE_FACTOR * scale; 
  rect.h = tilesetRect.w * final_scale;
  rect.w = tilesetRect.h * final_scale;
  rect.x -= sprite->pivot_x * final_scale;
  rect.y -= sprite->pivot_y * final_scale;
  rect.x -= camera->camera_x;
  rect.y -= camera->camera_y;
  
  SDL_SetTextureScaleMode(sprite->texture, SDL_SCALEMODE_PIXELART);
  SDL_SetTextureAlphaModFloat(sprite->texture, alpha);
  SDL_FlipMode flip = flipped ? SDL_FlipMode::SDL_FLIP_HORIZONTAL : SDL_FlipMode::SDL_FLIP_NONE;
  SDL_RenderTextureRotated(renderer, sprite->texture, &tilesetRect, &rect, 0, 0, flip);
}

void RenderTile(Sprite* tileset, int cell_id, LevelData* level, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale, float alpha){
  camera::GridToWorld(&x, &y, level);
  RenderSprite_World({cell_id, tileset} ,renderer, camera, x, y, scale, alpha, false);
}

void RenderSprite_OnTile(SpriteRenderInfo spriteInfo, LevelData* level, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale, float alpha, bool flipped){
  camera::GridToWorld(&x, &y, level);
  x += TILE_SIZE_PX_SCALED / 2.0;
  y += TILE_SIZE_PX_SCALED / 2.0;
  RenderSprite_World(spriteInfo,renderer, camera, x, y, scale, alpha, flipped);
}
