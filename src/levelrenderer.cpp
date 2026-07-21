#include "levelRenderer.h"
#include "common.h"
#include "entity.h"
#include "rendering.h"
#include "spriteLibrary.h"
#include <cmath>

void RenderLevel(GameData* gameData, SDL_Renderer* renderer){

  LevelData level = gameData->levels[gameData->currentLevelIndex];

  for(int x = 0; x < level.w; x++){
    for (int y = 0 ; y < level.h; y++) {
      uint8_t cellType = GetCellID(&level ,x, y);
      Sprite* sprite;
      
      if(ID(cellType) == ID::GROUND){
        sprite = &gameData->spriteBuffer[(x + y) % 2 == 0 ? (int)SPRITE_ID::Ground : (int)SPRITE_ID::Ground_alt];
        
      }
      else{
        sprite = GetSpriteFromID((ID)cellType, gameData->spriteBuffer);
      }
      RenderSprite_Grid(sprite, &level, renderer, &gameData->camera, x, y);
    }
  }
}

void RenderEntities(GameData* data, SDL_Renderer* renderer){
  LevelData lvl = data->levels[data->currentLevelIndex];
  for (int i = 0; i < lvl.entityCount; i++) {
    Entity entity = lvl.entityBuffer[i];
    if(entity.id == ID::NONE){
      continue;
    }
    Sprite* sprite = GetSprite_FromEntityState(&entity, data->spriteBuffer);
    if(HasBehaviour(&entity, Behaviour::IS_PETRIFIED)){
      sprite = GetSpriteFromID(ID::ROCK, data->spriteBuffer);
    }
    float x_animated = std::lerp(entity.x_prev, entity.x, entity.progress_01);
    float y_animated = std::lerp(entity.y_prev, entity.y, entity.progress_01);
    float dropshadow_y = y_animated;
    if(HasBehaviour(&entity, Behaviour::JUMPS) && !HasBehaviour(&entity, Behaviour::IS_PUSHING)){
     y_animated -= 0.5 * sinf(entity.progress_01 * 3.14);
    }

    Sprite* dropshadow = &data->spriteBuffer[(int)SPRITE_ID::Dropshadow];

    RenderEntity_OnTile(dropshadow, &lvl, renderer, &data->camera, x_animated, dropshadow_y, 1, 0.4, false);
    RenderEntity_OnTile(sprite, &lvl, renderer, &data->camera, x_animated, y_animated, 1, 1, entity.facing == Direction::RIGHT);
  }        
}
