
#include "spriteLibrary.h"
#include "SDL3_Image/SDL_image.h"
#include "common.h"
#include "entity.h"
#include <cassert>
#include <cmath>

using namespace std;


const char* FALLBACK_PATH = "assets/sprites/fallback.png";

static const SpriteDataEntry all_sprite_data[] = {
  {SPRITE_ID::Fallback, FALLBACK_PATH,8,8},
  {SPRITE_ID::Demon, "assets/sprites/player.png"},
  {SPRITE_ID::Rock, "assets/sprites/rock.png", 10, 18},
  {SPRITE_ID::Medusa_Rotate, "assets/sprites/medusa_rotate.png", 12, 24, 8, 1},
{
  .id = SPRITE_ID::Medusa_Idle_Left,
  .path = "assets/sprites/medusa_idle_left.png",
  .pivot_x = 12,
  .pivot_y = 24,
  .tileset_cell_count_x = 4,
  .tileset_cell_count_y = 1,
  .framerate = 8
},
  {SPRITE_ID::Medusa_Idle_Front, "assets/sprites/medusa_idle_front.png", 12, 24, 4, 1, 8},
  {SPRITE_ID::Medusa_Idle_Back, "assets/sprites/medusa_idle_back.png", 12, 24, 4, 1, 8},
  {SPRITE_ID::Dropshadow, "assets/sprites/dropshadow.png", 8, 8},
  {SPRITE_ID::black_1x1, "assets/sprites/1x1_black.png",0,0},
  {SPRITE_ID::titlescreen_background, "assets/sprites/titlescreen.png"},
  {SPRITE_ID::selection_marker, "assets/sprites/selection_marker.png",9,9},
  {SPRITE_ID::dungeon_tileset, "assets/sprites/hell_of_a_time_dungeon_tileset.png",0,0, 9, 9},
  {SPRITE_ID::Goal, "assets/sprites/goal.png",8, 8, 8, 1}
  };

SpriteRenderInfo GetSprite_FromEntityState(Entity* entity, Sprite* spritebuffer, const uint64_t* ticks_total){
    if(HasBehaviour(entity, Behaviour::IS_PETRIFIED)){
    return GetSprite(SPRITE_ID::Rock, spritebuffer);
  }
  
  if(entity->id == ENTITY_ID::MEDUSA && entity->action == Actions::ROTATING){
    Sprite* spritesheet = GetSprite(SPRITE_ID::Medusa_Rotate, spritebuffer);
     
    int start = 0;
    int end = 0;
    switch(entity->facing_previous){
    case Direction::RIGHT:
      start = 6;
      break;
    case Direction::LEFT:
      start = 2;
      break;
    case Direction::UP:
      start = 4;
      break;
    case Direction::DOWN:
      start = 0;
      break;
    }

    switch(entity->facing_current){
    case Direction::RIGHT:
      end = 6;
      break;
    case Direction::LEFT:
      end = 2;
      break;
    case Direction::UP:
      end = 4;
      break;
    case Direction::DOWN:
      end = 0;
      break;
    }

    int sprite_count = GetSpriteCount(spritesheet);
    int forward = ((end - start) % sprite_count + sprite_count) % sprite_count;
    int backward = sprite_count - forward;
    end = (forward <= backward) ? (start + forward) : (start - backward);
    int current_frame = (((int)lerp(start, end, entity->progress_01) + sprite_count) % sprite_count);
    return {current_frame, spritesheet};
  }

  switch (entity->id) {
  case ENTITY_ID::MEDUSA:{
  Sprite* sprite = nullptr;
  int frame = 0;
    switch (entity->facing_current) {
      case Direction::RIGHT:
        sprite = GetSprite(SPRITE_ID::Medusa_Idle_Left, spritebuffer);
        frame = (int)((*ticks_total * sprite->framerate) / FPS % GetSpriteCount(sprite));
        return {frame, sprite, true};
      case Direction::LEFT:
        sprite = GetSprite(SPRITE_ID::Medusa_Idle_Left, spritebuffer);
        frame = (int)((*ticks_total * sprite->framerate) / FPS % GetSpriteCount(sprite));
        return {frame, sprite};
      case Direction::DOWN:
        sprite = GetSprite(SPRITE_ID::Medusa_Idle_Back, spritebuffer);
        frame = (int)((*ticks_total * sprite->framerate) / FPS % GetSpriteCount(sprite));
        return {frame, sprite};
      case Direction::UP:
        sprite = GetSprite(SPRITE_ID::Medusa_Idle_Front, spritebuffer);
        frame = (int)((*ticks_total * sprite->framerate) / FPS % GetSpriteCount(sprite));
        return {frame, sprite};
      }
  }
  case ENTITY_ID::DEMON:
    return GetSprite(SPRITE_ID::Demon, spritebuffer);
  case ENTITY_ID::ROCK:
    return GetSprite(SPRITE_ID::Rock, spritebuffer);
  default:
    return GetSprite(SPRITE_ID::Fallback, spritebuffer);
  }  
}

Sprite* GetSprite(SPRITE_ID sprite_id, Sprite* spriteBuffer){
  Sprite* sprite = &spriteBuffer[(int)sprite_id];
  if(sprite == nullptr || sprite->texture == nullptr){
    assert(sprite_id != SPRITE_ID::Fallback);
    return GetSprite(SPRITE_ID::Fallback, spriteBuffer);
  }
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

    sprite->sprite_count_x = entry.tileset_cell_count_x;
    sprite->sprite_count_y = entry.tileset_cell_count_y;
    sprite->framerate = entry.framerate;
    
    SDL_DestroySurface(surface);
  }
}
