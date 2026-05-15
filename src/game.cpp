#include "game.h"
#include "SDL3/SDL_render.h"
#include "entityrenderer.h"
#include "gameState.h"
#include "levels.h"
#include "image.h"
#include "levelRenderer.h"

extern "C" {


  void Initialize(GameData* data, SDL_Renderer* renderer){
    data->ground = AssetManagement::LoadSprite(data->arena_images, renderer, "ground.png");
    data->wall   = AssetManagement::LoadSprite(data->arena_images, renderer, "wall.png"); 
    data->player = AssetManagement::LoadSprite(data->arena_images, renderer, "player.png");

    data->currentLevel = 0;
    CreateLevel(data->arena_levels, &data->levels[0], "assets/levels/testLevel.tmj");
    CreateEntities(&data->levels[data->currentLevel], data->arena_entities);  
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

    const bool* keys = SDL_GetKeyboardState(NULL);

       
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
