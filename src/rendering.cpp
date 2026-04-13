#include "rendering.h"
#include "SDL3/SDL_render.h"

void RenderSprite(Image* sprite, SDL_Renderer* renderer, int xPos, int yPos, float scale){
  SDL_FRect rect;
  rect.x = xPos;
  rect.y = yPos;
  rect.h = sprite->height * scale;
  rect.w = sprite->width * scale;
  
  SDL_RenderTexture(renderer, sprite->texture, NULL, &rect);
}
