#pragma once
#include "arena.h"
#include "entity.h"
#include "tilesetLibrary.h"
#include <cstdint>
using namespace Memory;

struct LevelData{
  int w;
  int h;
  uint16_t* cells;
  const char* level_path;
  Entity* entityBuffer;
  int entityCount;
  const Tileset* tileset;
};


bool IsWalkable(int x, int y, LevelData* level);
void CreateLevel(Arena* arena, LevelData* level, Tileset* tileset, const char* level_name);
void CreateEntities(LevelData* lvl_data, Arena* arena);
Entity* GetNextAvailableEntity(Entity* entityBuffer, int bufferSize);
void AddEntity(ENTITY_ID entity_id, int x, int y, LevelData* level);
void RemoveEntity(int x, int y, LevelData* level);
uint16_t GetCellID(LevelData* level ,int x, int y);
Entity* GetEntity(LevelData* level, int x, int y);
Entity* RaycastFirstEntity(int x_origin, int y_origin, Direction direction, LevelData* level, bool ignore_walls = false);
