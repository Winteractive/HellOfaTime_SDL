#pragma once
#include "SDL3/SDL_scancode.h"

struct Input{
  const bool* keys_current;
  const bool* keys_previous;
  float* keys_held_time;
};

bool KeyPressed(const Input* input,SDL_Scancode key);
bool KeyHeld(const Input* input, SDL_Scancode key);
bool KeyReleased(const Input* input,SDL_Scancode key);
bool KeyHeld_ForTime(const Input* input, SDL_Scancode key, float min_length);
void UpdateKeys(Input* input, float dt);
void ResetKeyHeldTime(Input* input, SDL_Scancode key);
void ResetAll(Input* input);
