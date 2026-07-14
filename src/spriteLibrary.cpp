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
 switch (id) {
 case ID::NONE:
   return nullptr;
 case ID::GROUND:
   return &spriteBuffer[(int)SPRITE_ID::Ground];
 case ID::WALL:
   return &spriteBuffer[(int)SPRITE_ID::Wall];
 case ID::DEMON:
   return &spriteBuffer[(int)SPRITE_ID::Demon];
 case ID::ROCK:
   return &spriteBuffer[(int)SPRITE_ID::Rock];
 case ID::MEDUSA:
   return &spriteBuffer[(int)SPRITE_ID::Medusa];
 case ID::GHOST:
   return &spriteBuffer[(int)SPRITE_ID::Ghost];
 case ID::GOLEM:
   return &spriteBuffer[(int)SPRITE_ID::Golem];
   break;
 default:
    return &spriteBuffer[(int)SPRITE_ID::Fallback];
   break;
 }
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
