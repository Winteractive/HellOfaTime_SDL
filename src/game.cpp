#include "game.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "entity.h"
#include "entityrenderer.h"
#include "gameState.h"
#include "levels.h"
#include "image.h"
#include "levelRenderer.h"

bool KeyPressed(SDL_Scancode key, const bool* current, const bool* previous){
  if(previous == nullptr){
    return current[key];
  }
  return current[key] && !previous[key];
}
bool KeyHeld(SDL_Scancode key, const bool* current, const bool* previous){
  if(previous == nullptr){
    return false;
  }
  return current[key] && previous[key];
}
bool KeyReleased(SDL_Scancode key, const bool* current, const bool* previous){
  if(previous == nullptr){
    return false;
  }
  return !current[key] && previous[key];
}

extern "C" {

  void Initialize(GameData* data, SDL_Renderer* renderer){
    data->ground = AssetManagement::LoadSprite(data->arena_images, renderer, "ground.png");
    data->wall   = AssetManagement::LoadSprite(data->arena_images, renderer, "wall.png"); 
    data->player = AssetManagement::LoadSprite(data->arena_images, renderer, "player.png");

    data->currentLevelIndex = 0;
    CreateLevel(data->arena_levels, &data->levels[0], "assets/levels/testLevel.tmj");
    CreateEntities(&data->levels[data->currentLevelIndex], data->arena_entities);  
  }

  bool HandleEvents(GameData *data, SDL_Event event){
    if(event.type != SDL_EVENT_KEY_DOWN){
        return true;
    }
    if(event.key.key == SDLK_ESCAPE){
        return false;
    }

     return true;
  } 
  
  
  void Update(GameData* data,float dt){

    const bool* keys = SDL_GetKeyboardState(nullptr);

    if(KeyPressed(SDL_SCANCODE_R, keys, data->keys_previous)){
      CreateEntities(data->GetCurrentLevel(), data->arena_entities);      
    }

    for (int i = 0; i < data->GetCurrentLevel()->entityCount; i++) {
      Entity* entity = &data->GetCurrentLevel()->entityBuffer[i];
      

      if(entity->HasBehaviour((Behaviour)(Behaviour::RESPOND_TO_INPUT | Behaviour::CAN_MOVE))){
        int xChange = 0;
        int yChange = 0;
        if(KeyPressed(SDL_SCANCODE_RIGHT, keys, data->keys_previous)){
          xChange = 1;
        }
        else if(KeyPressed(SDL_SCANCODE_LEFT, keys, data->keys_previous)){
          xChange = -1;
        }
        else if(KeyPressed(SDL_SCANCODE_UP, keys, data->keys_previous)){
          yChange = -1;
        }
        else if(KeyPressed(SDL_SCANCODE_DOWN, keys, data->keys_previous)){
          yChange = 1;
        }

        if(xChange != 0 || yChange != 0){
          int stepInto_x = entity->x + xChange;
          int stepInto_y = entity->y + yChange;
          Entity* stepInto_entity = data->GetCurrentLevel()->GetEntity(stepInto_x, stepInto_y);
          uint8_t stepInto_tile_id = data->GetCurrentLevel()->GetCellID(stepInto_x, stepInto_y);
          if(stepInto_entity == nullptr){
            if(stepInto_tile_id == (uint8_t)ID::GROUND){
              entity->x = stepInto_x;
              entity->y = stepInto_y;
            }
          }
        }
      }
    }

  memcpy((void*)data->keys_previous, keys, SDL_SCANCODE_COUNT * sizeof(bool));
         
  }
 
  void Draw(GameData* data, SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, 120, 70, 120, 255);
    SDL_RenderClear(renderer);

    RenderLevel(data, renderer);  
    RenderEntities(data, renderer);
        
    SDL_RenderPresent(renderer);
  }

  void OnQuit(SDL_Renderer* renderer){
    SDL_DestroyRenderer(renderer);
  }
   

}
