#pragma once

#include "levels.h"

struct Camera{
  float camera_x;
  float camera_y;
};
namespace camera {
void GridToWorld(float* x, float* y, const LevelData* lvl);
void WorldToGrid(float x_world, float y_world, int* x, int* y, const LevelData* lvl);
bool GetIsPointInsideGrid(float x, float y, const LevelData* lvl);
};
