#pragma once
#include "SDL3/SDL_render.h"
#include "arena.h"

struct Image{
  SDL_Texture* texture;
  int width;
  int height;
};

namespace AssetManagement
{
  Image* LoadSprite(Memory::Arena* arena, SDL_Renderer* renderer, const char* path);
}

