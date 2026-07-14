#include "levelRenderer.h"
#include "common.h"
#include "rendering.h"
#include "spriteLibrary.h"
#include <cmath>

void RenderLevel(GameData* gameData, SDL_Renderer* renderer){

  LevelData lvl = gameData->levels[gameData->currentLevelIndex];

  for(int x = 0; x < lvl.w; x++){
    for (int y = 0 ; y < lvl.h; y++) {
      uint8_t cellType = lvl.GetCellID(x, y);
      Sprite* sprite = GetSpriteFromID((ID)cellType, gameData->spriteBuffer);
      RenderSprite_Grid(sprite, &lvl, renderer, &gameData->camera, x, y);
    }
  }
}

void RenderEntities(GameData* data, SDL_Renderer* renderer){
  LevelData lvl = data->levels[data->currentLevelIndex];
  loop(i, lvl.entityCount){
    Entity entity = lvl.entityBuffer[i];
    if(entity.id == ID::NONE){
      continue;
    }
    Sprite* sprite = GetSpriteFromID(entity.id, data->spriteBuffer);
    
    float x_animated = std::lerp(entity.x_prev, entity.x, entity.progress_01);
    float y_animated = std::lerp(entity.y_prev, entity.y, entity.progress_01);
    
    RenderSprite_Grid(sprite, &lvl, renderer, &data->camera, x_animated, y_animated);
  }        
}
