#include "spriteLibrary.h"
#include "SDL3_Image/SDL_image.h"
#include "entity.h"
#include <cassert>

using namespace std;


const char* FALLBACK_PATH = "assets/sprites/fallback.png";

static const SpriteDataEntry all_sprite_data[] = {
  {SPRITE_ID::Fallback, FALLBACK_PATH,0,0},
  {SPRITE_ID::Demon, "assets/sprites/player.png"},
  {SPRITE_ID::Rock, "assets/sprites/rock.png", 10, 20},
  {SPRITE_ID::Medusa_Idle_Side, "assets/sprites/medusa_idle_side.png", 12, 24},
  {SPRITE_ID::Medusa_Idle_Front, "assets/sprites/medusa_idle_front.png", 12, 24},
  {SPRITE_ID::Medusa_Idle_Back, "assets/sprites/medusa_idle_back.png", 12, 24},
  {SPRITE_ID::Dropshadow, "assets/sprites/dropshadow.png", 8, 8},
  {SPRITE_ID::black_1x1, "assets/sprites/1x1_black.png",0,0},
  {SPRITE_ID::titlescreen_background, "assets/sprites/titlescreen.png",0,0},
  {SPRITE_ID::dungeon_tileset, "assets/sprites/hell_of_a_time_dungeon_tileset.png",0,0, 9, 9}
};

Sprite* GetSprite_FromEntityState(Entity* entity, Sprite* spritebuffer){
  if(HasBehaviour(entity, Behaviour::IS_PETRIFIED)){
    return &spritebuffer[(int)SPRITE_ID::Rock];
  }

  switch (entity->id) {
  case ENTITY_ID::MEDUSA:
    switch (entity->facing) {
    case Direction::RIGHT:
    case Direction::LEFT:
      return &spritebuffer[(int)SPRITE_ID::Medusa_Idle_Side];
      break;
    case Direction::DOWN:
      return &spritebuffer[(int)SPRITE_ID::Medusa_Idle_Back];
      break;
    case Direction::UP:
      return &spritebuffer[(int)SPRITE_ID::Medusa_Idle_Front];
      break;
    }
    default:
    return GetSpriteFromID(entity->id, spritebuffer);
    break;
  }

  return nullptr;
}

Sprite* GetSprite(SPRITE_ID sprite_id, Sprite* spriteBuffer){
  return &spriteBuffer[(int)sprite_id];
}

Sprite* GetSpriteFromID(ENTITY_ID id, Sprite* spriteBuffer){
  Sprite* sprite_to_return = nullptr;
  
 switch (id) {
 case ENTITY_ID::DEMON:
   sprite_to_return = &spriteBuffer[(int)SPRITE_ID::Demon];
   break;
 case ENTITY_ID::ROCK:
   sprite_to_return = &spriteBuffer[(int)SPRITE_ID::Rock];
   break;
 case ENTITY_ID::MEDUSA:
   sprite_to_return = nullptr;
   break;
 case ENTITY_ID::SIREN:
   sprite_to_return = &spriteBuffer[(int)SPRITE_ID::Siren];
   break;
 case ENTITY_ID::GOLEM:
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
    if(entry.pivot_x == NOT_SET || entry.pivot_y == NOT_SET){
      sprite->pivot_x = sprite->width / 2;
      sprite->pivot_y = sprite->height / 2;
    }
    else{
      sprite->pivot_x = entry.pivot_x;
      sprite->pivot_y = entry.pivot_y;
    }

    sprite->tileset_cell_count_x = entry.tileset_cell_count_x;
    sprite->tileset_cell_count_y = entry.tileset_cell_count_y;

    
    SDL_DestroySurface(surface);
  }
}
