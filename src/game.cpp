#include "game.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_render.h"

float xPos;
float yPos;
constexpr float SPEED = 100.0f;
SDL_FRect box;

void Core::Initialize(){
  xPos = 100;
  yPos = 100;
  box.h = 50;
  box.w = 50;
  box.x = xPos;
  box.y = yPos;
}
  
void Core::Update(float dt){

  const bool* keys = SDL_GetKeyboardState(NULL);

  if(keys[SDL_SCANCODE_RIGHT]){
    xPos += SPEED * dt;
  }
   
  if(keys[SDL_SCANCODE_LEFT]){
    xPos -= SPEED * dt;
  }

  if(keys[SDL_SCANCODE_UP]){
      yPos -= SPEED * dt;
  }

  if(keys[SDL_SCANCODE_DOWN]){
    yPos += SPEED * dt;
  }

  box.x = xPos;
  box.y = yPos;
    
}

void Core::Draw(SDL_Renderer* renderer){
  SDL_SetRenderDrawColor(renderer, 0, 70, 0, 255);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 150, 0, 30, 255);
  SDL_RenderFillRect(renderer,&box);

  SDL_RenderPresent(renderer);
}

void Core::OnQuit(SDL_Renderer* renderer){
  SDL_DestroyRenderer(renderer);
}
   

