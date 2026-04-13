#pragma once
#include "SDL3/SDL_render.h"
#include "image.h"

void RenderSprite(Image* sprite, SDL_Renderer* renderer, int xPos, int yPos, float scale = 1);
