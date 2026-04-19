#include "game.h"
#include "SDL3/SDL_render.h"
#include "gameState.h"
#include "rendering.h"

extern "C" {

  void Initialize(GameData* data){
    data->rect.x = 100;
    data->rect.y = 100;
    data->rect.h = 50;
    data->rect.w = 50;
    data->angle = 0;
    data->move_speed = 100;
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

    if(keys[SDL_SCANCODE_RIGHT]){
      data->rect.x += data->move_speed * dt;
    }
   
    if(keys[SDL_SCANCODE_LEFT]){
      data->rect.x -= data->move_speed * dt;
    }

    if(keys[SDL_SCANCODE_UP]){
        data->rect.y -= data->move_speed * dt;
    }

    if(keys[SDL_SCANCODE_DOWN]){
      data->rect.y += data->move_speed * dt;
    }


      // data->angle += 15 * dt;
      // float radius = 100;
      // data->rect.x = 300 + cosf(data->angle) * radius;
      // data->rect.y = 200 + sinf(data->angle) * radius;
    
  }


  void Draw(GameData* data, SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, 0, 70, 8, 255);
    SDL_RenderClear(renderer);
    RenderSprite(data->fallback, renderer, data->rect.x, data->rect.y);
    SDL_RenderPresent(renderer);
  }

  void OnQuit(SDL_Renderer* renderer){
    SDL_DestroyRenderer(renderer);
  }
   

}
