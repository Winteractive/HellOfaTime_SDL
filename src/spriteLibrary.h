#pragma once

#include "SDL3/SDL_render.h"
#include "entity.h"

enum class SPRITE_ID{
  Fallback,
  Ground,
  Ground_alt,
  Wall,
  Rock,
  Demon,
  Medusa_Idle_Side,
  Medusa_Idle_Front,
  Medusa_Idle_Back,
  Golem,
  Siren,
  Dropshadow,
  titlescreen_background,
  black_1x1
};

struct Sprite{
  SDL_Texture* texture;
  int width;
  int height;
  int pivot_x;
  int pivot_y;
};

const int NOT_SET = -1;
struct SpriteDataEntry{
  SPRITE_ID id;
  const char* path;
  int pivot_x = NOT_SET;
  int pivot_y = NOT_SET;
};


Sprite* GetSprite(SPRITE_ID sprite_id, Sprite* spriteBuffer);
Sprite* GetSprite_FromEntityState(Entity* entity, Sprite* spritebuffer);
Sprite* GetSpriteFromID(ID id, Sprite* spriteBuffer);

namespace AssetManagement
{
  void LoadSprite(Sprite* spriteBuffer, SpriteDataEntry entry, SDL_Renderer* renderer);
  void LoadAllSprites(Sprite* spriteBuffer, SDL_Renderer* renderer);
}
