#include "camera.h"
#include "common.h"

bool camera::GetIsPointInsideGrid(float x, float y, const LevelData* lvl){
  int x_grid;
  int y_grid;
  WorldToGrid(x, y, &x_grid, &y_grid, lvl);
  return x_grid >= 0 && y_grid >= 0 && x_grid < lvl->w && y_grid < lvl->h; 
}

void camera::GridToWorld(float* x, float* y,  const LevelData* lvl){
  *x *= CELL_SIZE_PX;
  *x += SCREEN_WIDTH / 2.0;
  *x -= lvl->w * CELL_SIZE_PX / 2.0;
  *y *= CELL_SIZE_PX;
  *y += SCREEN_HEIGHT / 2.0;
  *y -= lvl->h * CELL_SIZE_PX / 2.0;
}
void camera::WorldToGrid(float x_world, float y_world, int* x, int* y, const LevelData* lvl){
  *x = x_world;
  *y = y_world;
  *x += lvl->w * CELL_SIZE_PX / 2.0;
  *x -= SCREEN_WIDTH / 2.0;
  *x /= CELL_SIZE_PX;
  *y += lvl->h * CELL_SIZE_PX / 2.0;
  *y -= SCREEN_HEIGHT / 2.0;
  *y /= CELL_SIZE_PX;
}
