#pragma once

#define KILOBYTES(n) ((size_t)n * 1024)
#define MEGABYTES(n) (KILOBYTES(n) * 1024)
#define GIGABYTES(n) (MEGABYTES(n) * 1024)

#define loop(i, e) for(int i = 0; i < e; i++)


constexpr size_t GAME_MEMORY_ALLOWANCE = MEGABYTES(10);
constexpr int FPS = 240;
const double FRAME_TIME_MS = 1000.0 / FPS;

const float UNDO_REPEAT_TIME = 0.15f;
const float MOVE_SPEED = 6.0;
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int UPSCALE_FACTOR = 2;
const int CELL_SIZE_PX = 32 * UPSCALE_FACTOR;
