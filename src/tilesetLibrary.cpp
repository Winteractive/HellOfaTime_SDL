#include "Parsers/json.hpp"
#include "tilesetLibrary.h"
#include "arena.h"
#include <cassert>
#include "fstream"

using namespace nlohmann;
using namespace std;

static const TilesetDataEntry all_tilesets_data[]{
  {TILESETS::Dungeon, "assets/tilesets/dungeon_tileset.tsj"}
};


uint16_t Get_Tileset_ID_Offset_From_Tilemap(int id_limit, const json& tmj_result){
  int highest_tilemap_start_id = 0;
  for (const json& tileset : tmj_result["tilesets"]) {
    int first_id = tileset["firstgid"].get<int>();
    if(first_id <= id_limit && first_id > highest_tilemap_start_id){
      highest_tilemap_start_id = first_id;
    }
  }
  return highest_tilemap_start_id;
}

uint16_t GetLocalTileID(uint16_t id_global, const json& tmj_result){
  return id_global - Get_Tileset_ID_Offset_From_Tilemap(id_global, tmj_result);
}


namespace AssetManagement{
  void LoadAllTilesets(Tileset* tilesetBuffer, Memory::Arena* arena_images){
    for (TilesetDataEntry entry : all_tilesets_data) {
      LoadTileset(&entry, tilesetBuffer, arena_images);
    }    
  }
  
  void LoadTileset(TilesetDataEntry* entry, Tileset* tilesetBuffer, Memory::Arena* arena_images){
    assert(entry->type != TILESETS::COUNT);
    assert(entry->type != TILESETS::NONE);
    
    Tileset* tileset = &tilesetBuffer[(int)entry->type];
    tileset->type = entry->type;
    fstream stream(entry->path);
    auto jsonResult = json::parse(stream);
    int tile_count = jsonResult["tilecount"].get<int>();
    tileset->walkableBuffer = ALLOC_ARRAY(arena_images, bool, tile_count);
    
    auto& tiles = jsonResult["tiles"];

    for(const auto& tile : tiles){
      int tile_id = tile["id"].get<int>();

      for(const auto& tile_property : tile["properties"]){
        if(tile_property["name"] == "walkable"){
          tileset->walkableBuffer[tile_id] = tile_property["value"].get<bool>();
        }
      }
    }
  }
}
