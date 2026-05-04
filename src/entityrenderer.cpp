#include "entityrenderer.h"
#include "common.h"
#include "gameState.h"
#include "rendering.h"

void RenderEntities(GameData* data, SDL_Renderer* renderer){
  LevelData lvlData = data->levels[data->currentLevel];
  loop(i, lvlData.entityCount){
    Image* img;
    Entity entity = lvlData.entityBuffer[i];
    switch(entity.id){
      case 3:
        img = data->player;
        break;
      default:
        img = data->fallback;
        break;
    }

    int xPos = 0;
    int yPos = 0;

    xPos += SCREEN_WIDTH  / 2;
    yPos += SCREEN_HEIGHT / 2;
    
    xPos -= data->levels[data->currentLevel].w * CELL_SIZE_PX / 2;
    yPos -= data->levels[data->currentLevel].h * CELL_SIZE_PX / 2;

    xPos += entity.x * CELL_SIZE_PX;
    yPos += entity.y * CELL_SIZE_PX;
    
    RenderSprite(img, renderer, xPos, yPos, UPSCALE_FACTOR);
  }        
}
