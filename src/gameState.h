#pragma once
#include "SDL3/SDL_rect.h"
#include "image.h"

  struct GameData {
  SDL_FRect rect;
  float move_speed;
  Image* fallback;
  float angle;
};

