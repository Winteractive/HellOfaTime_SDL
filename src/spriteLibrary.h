#pragma once

#include "SDL3/SDL_render.h"
#include "entity.h"

enum class SPRITE_ID{
  Fallback,
  Ground,
  Wall,
  Rock,
  Demon,
  Medusa,
  Golem,
  Ghost
};

struct Sprite{
  SDL_Texture* texture;
  int width;
  int height;
};

struct SpriteDataEntry{
  SPRITE_ID id;
  const char* path;
};


Sprite* GetSpriteFromID(ID id, Sprite* spriteBuffer);

namespace AssetManagement
{
  void LoadSprite(Sprite* spriteBuffer, SpriteDataEntry entry, SDL_Renderer* renderer);
  void LoadAllSprites(Sprite* spriteBuffer, SDL_Renderer* renderer);
}
