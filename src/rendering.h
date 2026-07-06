#pragma once
#include "SDL3/SDL_render.h"
#include "camera.h"
#include "image.h"
#include "levels.h"

void RenderSprite_World(Image* sprite, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale = 1);
void RenderSprite_Grid(Image* sprite, LevelData* lvl, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale = 1);
