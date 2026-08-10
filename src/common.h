#pragma once

#define KILOBYTES(n) ((size_t)n * 1024)
#define MEGABYTES(n) (KILOBYTES(n) * 1024)
#define GIGABYTES(n) (MEGABYTES(n) * 1024)

#define loop(i, e) for(int i = 0; i < e; i++)

constexpr size_t GAME_MEMORY_ALLOWANCE = MEGABYTES(10);
constexpr int FPS = 240;
const double FRAME_TIME_MS = 1000.0 / FPS;

const float UNDO_REPEAT_TIME = 0.15;
const float MOVE_SPEED = 6.0;
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int UPSCALE_FACTOR = 4;
const int TILE_SIZE_PX_RAW = 16;
const int TILE_SIZE_PX_SCALED = TILE_SIZE_PX_RAW * UPSCALE_FACTOR;

inline void Expand1DTo2D(int flatIndex, int width, int* x, int* y){
    *x = flatIndex % width;
    *y = flatIndex / width;
}
inline void Expand1DTo2D(int flatIndex, int width, float* x, float* y){
    *x = (float)(flatIndex % width);
    *y = (float)(flatIndex / width);
}
