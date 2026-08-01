#pragma once
#include "Parsers/json.hpp"

using namespace nlohmann;

namespace Memory{
  struct Arena;
}
 
enum class TILESETS {
  NONE = 0,
  Dungeon = 1,
  COUNT = 2
};

struct Tileset{
  TILESETS type;
  bool* walkableBuffer;
};

struct TilesetDataEntry{
  TILESETS type;
  const char* path;
};

uint16_t GetLocalTileID(uint16_t id_global, const json& tmj_result);
uint16_t Get_Tileset_ID_Offset_From_Tilemap(int id_limit, const json& tmj_result);
  
namespace AssetManagement{
  void LoadAllTilesets(Tileset* tilesetBuffer, Memory::Arena* arena_images);
  void LoadTileset(TilesetDataEntry* entry, Tileset* tilesetBuffer, Memory::Arena* arena_images);
}
