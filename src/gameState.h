#pragma once
#include "arena.h"
#include "image.h"
#include "levels.h"

struct GameData {
  Image* fallback;
  Image* wall;
  Image* ground;
  Image* player;
  Memory::Arena* arena_levels;
  Memory::Arena* arena_entities;
  Memory::Arena* arena_images;
  bool* keys_previous;
  LevelData* levels;
  int levelCount;
  int currentLevelIndex;

  LevelData* GetCurrentLevel(){
    return &levels[currentLevelIndex];
  }
};
