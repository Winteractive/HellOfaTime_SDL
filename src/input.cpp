#include "input.h"
#include <cstring>

bool KeyPressed(const Input* input,SDL_Scancode key){
  if(input->keys_previous == nullptr){
    return input->keys_current[key];
  }
  return input->keys_current[key] && !input->keys_previous[key];
}

bool KeyHeld(const Input* input, SDL_Scancode key){
  if(input->keys_previous == nullptr){
    return false;
  }
  return input->keys_current[key] && input->keys_previous[key];
}

bool KeyReleased(const Input* input,SDL_Scancode key){
  if(input->keys_previous == nullptr){
    return false;
  }
  return !input->keys_current[key] && input->keys_previous[key];
}

bool KeyHeld_ForTime(const Input* input, SDL_Scancode key, float min_length){
  return input->keys_held_time[key] >= min_length; 
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
 
void ResetAll(Input* input){
  memset((void*)input->keys_current, 0, sizeof(bool) * SDL_SCANCODE_COUNT);
  memset((void*)input->keys_previous, 0, sizeof(bool) * SDL_SCANCODE_COUNT);
  memset((void*)input->keys_held_time, 0, sizeof(float) * SDL_SCANCODE_COUNT);
}
