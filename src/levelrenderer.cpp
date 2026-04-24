#include "levelRenderer.h"
#include "common.h"
#include "rendering.h"
#include <cstdint>


void RenderLevel(LevelData lvl, GameData* gameData, SDL_Renderer* renderer){

  int board_width =  lvl.w * 32 / 2;
  int board_height = lvl.h * 32 / 2;

  for(int x = 0; x < lvl.w; x++){
    for (int y = 0 ; y < lvl.h; y++) {
      uint8_t cellType = lvl.GetCellID(x, y);
      float xPos = x * 32;
      float yPos = y * 32;
    
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

      xPos -= board_width;
      yPos -= board_height;
      
      RenderSprite(sprite, renderer, xPos, yPos);
    }
  }
}
