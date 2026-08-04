#include <cstdint>
#include <fstream>
#include <vector>
#include "levels.h"
#include "arena.h"
#include "Parsers/json.hpp"
#include "entity.h"
#include "tilesetLibrary.h"
using namespace std;
using namespace nlohmann;

void CreateLevel(Arena* arena, LevelData* level, Tileset* tileset, const char* level_name){
  fstream stream(level_name);
  auto result = json::parse(stream);

  bool found = false;
  vector<uint16_t> levelData;
  for (const auto& layer : result["layers"]) {
      if (layer["name"] == "level") {
          levelData = layer["data"].get<vector<uint16_t>>();
          found = true;
          break;
      }
  }
  assert(found);
  int first_non_zero_id = 0;
  for (int id : levelData) {
    if(id != 0){
      first_non_zero_id = id;
      break;
    }
  } 
  int id_offset = Get_Tileset_ID_Offset_From_Tilemap(first_non_zero_id, result);
  
  level->w = result["width"].get<int>();
  level->h = result["height"].get<int>();
  level->level_path = level_name;
  level->tileset = tileset;
  level->cells = ALLOC_ARRAY(arena, uint16_t, level->w * level->h);
  for (int i = 0; i < level->w * level->h; i++) {
    int local_id = levelData[i] - id_offset;
    if(local_id < 0){
      local_id = 0;
    }
    level->cells[i] = local_id;
  }
}

void CreateEntities(LevelData* lvl_data, Arena* arena){
  Reset(arena);
  lvl_data->entityCount = 0;
  lvl_data->entityBuffer = (Entity*)Memory::Allocate(arena, sizeof(Entity) * 256);
  
  fstream stream(lvl_data->level_path);
  auto result = json::parse(stream);

  vector<uint16_t> entities;
  bool found = false;
  for (const auto& layer : result["layers"]) {
      if (layer["name"] == "entities") {
          entities = layer["data"].get<vector<uint16_t>>();
          found = true;
          break;
      }
  }
  if(!found){
    return;
  }
    
  for (int i = 0; i < lvl_data->w * lvl_data->h; i++) {
    if(entities[i] == 0){
      continue;
    }
    uint16_t entity_id = GetLocalTileID(entities[i], result);
    int x = i % lvl_data->w;
    int y = i / lvl_data->w;
    AddEntity((ENTITY_ID)entity_id, x, y, lvl_data);
  }    
}

Entity* GetNextAvailableEntity(LevelData* level) {
  for (int i = 0; i < level->entityCount; i++) {
    if(level->entityBuffer[i].active == false){
      return &level->entityBuffer[i];
    }
  }

  return &level->entityBuffer[level->entityCount++];
}

void AddEntity(ENTITY_ID entity_id, int x, int y, LevelData *level){
  Entity* entity = GetEntity(level, x, y);
  
  if(entity == nullptr){
    entity = GetNextAvailableEntity(level);
  }

  entity->active = true;
  entity->x = x;
  entity->y = y;
  entity->x_prev = x;
  entity->y_prev = y;
  entity->id = entity_id;
  entity->action = Actions::NONE;
  InitializeBaseBehaviour(entity);
}

void RemoveEntity(int x, int y, LevelData* level){
  Entity* entity = GetEntity(level, x, y);
  if(entity == nullptr){
    return;
  }
  *entity = {};
}


bool IsWalkable(int x, int y, LevelData* level){
  uint16_t id = GetCellID(level, x, y);  
  return level->tileset->walkableBuffer[id];
}

 uint16_t GetCellID(LevelData* level, int x, int y){
    return level->cells[y * level->w + x];
  }

Entity* GetEntity(LevelData* level, int x, int y){
  for (int i = 0; i < level->entityCount; i++) {
    if(level->entityBuffer[i].x == x && level->entityBuffer[i].y == y){
      return &level->entityBuffer[i];
    }
  }

  return nullptr; 
}

Entity* RaycastFirstEntity(int x_origin, int y_origin, Direction direction, LevelData* level, bool ignore_walls){
  Position facingVector;
  switch (direction) {
  case Direction::RIGHT:
      facingVector = {1, 0};
      break;
  case Direction::LEFT:
      facingVector = {-1, 0};
      break;
  case Direction::UP:
      facingVector = {0, 1};
      break;
  case Direction::DOWN:
      facingVector = {0, -1};
      break;
  }

  int x_search = x_origin + facingVector.x;
  int y_search = y_origin + facingVector.y;

  while(x_search > 0 && x_search < level->w && y_search > 0 && y_search < level->h){
      if(!ignore_walls && !IsWalkable(x_search, y_search, level)){
        break;
      }

      Entity* entity_search = GetEntity(level, x_search, y_search);
      if(entity_search != nullptr){
        return entity_search;
      }

      x_search += facingVector.x;
      y_search += facingVector.y;
  }

  return nullptr;
}
