#pragma once

#define KILOBYTES(n) ((size_t)n * 1024)
#define MEGABYTES(n) (KILOBYTES(n) * 1024)
#define GIGABYTES(n) (MEGABYTES(n) * 1024)

constexpr size_t GAME_MEMORY_ALLOWANCE = MEGABYTES(10);
constexpr int FPS = 240;
const double FRAME_TIME_MS = 1000.0 / FPS;
