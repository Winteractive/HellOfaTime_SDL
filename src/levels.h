#pragma once
#include "arena.h"
#include "entity.h"
#include <cstdint>
using namespace Memory;


struct LevelData{
  int w;
  int h;
  uint8_t* cells;
  const char* level_path;
  Entity* entityBuffer;
  int entityCount;
  uint8_t GetCellID(int x, int y){
    return cells[y * w + x];
  }
};

void CreateLevel(Arena* arena, LevelData* level, const char* level_name);
void CreateEntities(LevelData* lvl_data, Arena* arena);
