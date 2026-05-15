#include <cstdint>
#include <fstream>
#include <vector>
#include "levels.h"
#include "arena.h"
#include "Parsers/json.hpp"
#include "entity.h"
using namespace std;

const int LEVEL_INDEX = 0;
const int ENTITIES_INDEX = 1;

void CreateLevel(Arena* arena, LevelData* level, const char* level_name){
  fstream stream(level_name);
  auto jsonResult = nlohmann::json::parse(stream);
  vector dataField = jsonResult["layers"][LEVEL_INDEX]["data"].get<vector<uint8_t>>();
  level->w = jsonResult["width"].get<int>();
  level->h = jsonResult["height"].get<int>();
  level->level_path = level_name;
  size_t size_of_cells = sizeof(uint8_t) * level->w * level->h;
  level->cells = (uint8_t*)Memory::Allocate(arena, size_of_cells);  
  for (int i = 0; i < level->w * level->h; i++) {
    level->cells[i] = dataField[i];
  }
}

void CreateEntities(LevelData* lvl_data, Arena* arena){
  Reset(arena);
  lvl_data->entityCount = 0;
  fstream stream(lvl_data->level_path);
  auto result = nlohmann::json::parse(stream);
  auto entityData = result["layers"][ENTITIES_INDEX]["data"].get<vector<uint8_t>>();

  for (int i = 0; i < lvl_data->w * lvl_data->h; i++) {
      unsigned char entity_id = entityData[i];
      if(entity_id != 0){
        lvl_data->entityCount++;
      }  
    }  

  lvl_data->entityBuffer = (Entity*)Memory::Allocate(arena, sizeof(Entity) * lvl_data->entityCount);
  int index = 0;
  for (int i = 0; i < lvl_data->w * lvl_data->h; i++) {
    unsigned char entity_id = entityData[i];
    if(entity_id != 0){
      int x = i % lvl_data->w;
      int y = i / lvl_data->w;
      lvl_data->entityBuffer[index].id = entity_id;
      lvl_data->entityBuffer[index].x  = x;
      lvl_data->entityBuffer[index].y  = y;
      index += 1;
    }  
  }
}

  

