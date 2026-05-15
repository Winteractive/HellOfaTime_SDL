#include "rendering.h"
#include "SDL3/SDL_render.h"
#include "common.h"

void RenderSprite(Image* sprite, SDL_Renderer* renderer, int xPos, int yPos, float scale){
  SDL_FRect rect;
  rect.x = xPos;
  rect.y = yPos;
  rect.h = sprite->height * UPSCALE_FACTOR * scale;
  rect.w = sprite->width * UPSCALE_FACTOR * scale;
  
  SDL_RenderTexture(renderer, sprite->texture, NULL, &rect);
}
