#include "spriteLibrary.h"
#include "SDL3_Image/SDL_image.h"
#include <cassert>

const char* FALLBACK_PATH = "assets/sprites/fallback.png";

static const SpriteDataEntry all_sprite_data[] = {
  {SPRITE_ID::Fallback, FALLBACK_PATH                  },
  {SPRITE_ID::Wall,     "assets/sprites/wall.png"      },
  {SPRITE_ID::Demon,    "assets/sprites/player.png"    },
  {SPRITE_ID::Rock,     "assets/sprites/box.png"       },
  {SPRITE_ID::Ground,   "assets/sprites/ground.png"    },
  {SPRITE_ID::Medusa,   "assets/sprites/medusa.png"    }
};


Sprite* GetSpriteFromID(ID id, Sprite* spriteBuffer){
  Sprite* sprite_to_return = nullptr;
  
 switch (id) {
 case ID::NONE:
   sprite_to_return = nullptr;
   break;
 case ID::GROUND:
   sprite_to_return = &spriteBuffer[(int)SPRITE_ID::Ground];
   break;
 case ID::WALL:
   sprite_to_return = &spriteBuffer[(int)SPRITE_ID::Wall];
   break;
 case ID::DEMON:
   sprite_to_return = &spriteBuffer[(int)SPRITE_ID::Demon];
   break;
 case ID::ROCK:
   sprite_to_return = &spriteBuffer[(int)SPRITE_ID::Rock];
   break;
 case ID::MEDUSA:
   sprite_to_return = &spriteBuffer[(int)SPRITE_ID::Medusa];
   break;
 case ID::SIREN:
   sprite_to_return = &spriteBuffer[(int)SPRITE_ID::Siren];
   break;
 case ID::GOLEM:
   sprite_to_return = &spriteBuffer[(int)SPRITE_ID::Golem];
   break;
  }

  if(sprite_to_return == nullptr || sprite_to_return->texture == nullptr){
    sprite_to_return = &spriteBuffer[(int)SPRITE_ID::Fallback];
  }

  return sprite_to_return;
}

namespace AssetManagement{
  void LoadAllSprites(Sprite* spriteBuffer, SDL_Renderer *renderer){
    for (SpriteDataEntry entry : all_sprite_data) {
      LoadSprite(spriteBuffer, entry, renderer);
    }
  }

  void LoadSprite(Sprite* spriteBuffer, SpriteDataEntry entry, SDL_Renderer* renderer){
    SDL_Surface* surface = IMG_Load(entry.path); 
    if(surface == nullptr){
      surface = IMG_Load(FALLBACK_PATH);
    }
    assert(surface != nullptr);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    Sprite* sprite = &spriteBuffer[(int)entry.id];
    sprite->texture = texture;
    sprite->height = texture->h;
    sprite->width = texture->w;
    
    SDL_DestroySurface(surface);
  }
}
