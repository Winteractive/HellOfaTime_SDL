#include "levelRenderer.h"
#include "common.h"
#include "levels.h"
#include "rendering.h"
#include <cmath>

void RenderLevel(GameData* gameData, SDL_Renderer* renderer){

  LevelData lvl = gameData->levels[gameData->currentLevelIndex];
  
  int board_width_px_half =  lvl.w * CELL_SIZE_PX / 2;
  int board_height_px_half = lvl.h * CELL_SIZE_PX / 2;

  for(int x = 0; x < lvl.w; x++){
    for (int y = 0 ; y < lvl.h; y++) {
      uint8_t cellType = lvl.GetCellID(x, y);
      float xPos = x * CELL_SIZE_PX;
      float yPos = y * CELL_SIZE_PX;
    
      Image* sprite;
      switch(cellType){
        case 1:
          sprite = gameData->ground;
          break;
        case 2:
          sprite = gameData->wall;
          break;
        default:
           sprite = gameData->fallback; 
          break;
      }

      xPos += SCREEN_WIDTH  / 2.0;
      yPos += SCREEN_HEIGHT / 2.0;

      xPos -= board_width_px_half;
      yPos -= board_height_px_half;
      
      RenderSprite(sprite, renderer, xPos, yPos);
    }
  }
}

void RenderEntities(GameData* data, SDL_Renderer* renderer){
  LevelData lvlData = data->levels[data->currentLevelIndex];
  loop(i, lvlData.entityCount){
    Image* img;
    Entity entity = lvlData.entityBuffer[i];
    switch(entity.id){
      case ID::PLAYER:
        img = data->player;
        break;
      case ID::BOX:
        img = data->box;
        break;
      default:
        img = data->fallback;
        break;
    }

    int xPos = 0;
    int yPos = 0;

    xPos += SCREEN_WIDTH  / 2.0;
    yPos += SCREEN_HEIGHT / 2.0;
    
    xPos -= data->levels[data->currentLevelIndex].w * CELL_SIZE_PX / 2;
    yPos -= data->levels[data->currentLevelIndex].h * CELL_SIZE_PX / 2;
        
    float x_animated = std::lerp(entity.x_prev, entity.x, entity.progress_01);
    float y_animated = std::lerp(entity.y_prev, entity.y, entity.progress_01);
    xPos += x_animated * CELL_SIZE_PX;
    yPos += y_animated * CELL_SIZE_PX;
    
    RenderSprite(img, renderer, xPos, yPos);
  }        
}
