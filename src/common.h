#pragma once

#define KILOBYTES(n) ((size_t)n * 1024)
#define MEGABYTES(n) (KILOBYTES(n) * 1024)
#define GIGABYTES(n) (MEGABYTES(n) * 1024)

#define loop(i, e) for(int i = 0; i < e; i++)


constexpr size_t GAME_MEMORY_ALLOWANCE = MEGABYTES(10);
constexpr int FPS = 240;
const double FRAME_TIME_MS = 1000.0 / FPS;

const int SCREEN_WIDTH = 650;
const int SCREEN_HEIGHT = 400;
const int UPSCALE_FACTOR = 2;
const int CELL_SIZE_PX = 32 * UPSCALE_FACTOR;
