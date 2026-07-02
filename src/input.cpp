#include "input.h"

bool KeyPressed(Input input,SDL_Scancode key){
  if(input.keys_previous == nullptr){
    return input.keys_current[key];
  }
  return input.keys_current[key] && !input.keys_previous[key];
}

bool KeyHeld_ForTime(Input input, SDL_Scancode key, float min_length){
  return input.keys_held_time[key] >= min_length; 
}

bool KeyHeld(Input input, SDL_Scancode key){
  if(input.keys_previous == nullptr){
    return false;
  }
  return input.keys_current[key] && input.keys_previous[key];
}

bool KeyReleased(Input input,SDL_Scancode key){
  if(input.keys_previous == nullptr){
    return false;
  }
  return !input.keys_current[key] && input.keys_previous[key];
}

void UpdateKeys(Input* input, float dt){
  for (int i = 0; i < SDL_SCANCODE_COUNT; i++) {
    if (input->keys_current[i]){
        input->keys_held_time[i] += dt;
    }
    else{
        input->keys_held_time[i] = 0;
    }
  }
  memcpy((void*)input->keys_previous, input->keys_current, SDL_SCANCODE_COUNT * sizeof(bool));
}

void ResetKeyHeldTime(Input* input,SDL_Scancode key){
  input->keys_held_time[key] = 0;
}
