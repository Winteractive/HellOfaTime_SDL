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

  Entity* GetEntity(int x, int y){
    for (int i = 0; i < entityCount; i++) {
      if(entityBuffer[i].x == x && entityBuffer[i].y == y){
        return &entityBuffer[i];
      }
    }
  
    return nullptr; 
  }
};

void CreateLevel(Arena* arena, LevelData* level, const char* level_name);
void CreateEntities(LevelData* lvl_data, Arena* arena);
Entity* GetNextAvailableEntity(Entity* entityBuffer, int bufferSize);
void AddEntity(ID entity_id, int x, int y, LevelData* level);
void RemoveEntity(int x, int y, LevelData* level);
