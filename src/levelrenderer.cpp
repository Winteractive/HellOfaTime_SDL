#include "levelRenderer.h"
#include "arena.h"
#include "entity.h"
#include "gameState.h"
#include "rendering.h"
#include "spriteLibrary.h"
#include <algorithm>
#include <cmath>

void RenderLevel(GameData* gameData, SDL_Renderer* renderer){
  Gameplay* gameplay = &gameData->scenes.gameplay;
  LevelData* level = &gameplay->levels[gameplay->currentLevelIndex];

  Sprite* tileset;
  switch(level->tileset->type){
    case TILESETS::Dungeon:
      tileset = GetSprite(SPRITE_ID::dungeon_tileset, gameData->spriteBuffer);
      break;
    case TILESETS::NONE:
    case TILESETS::COUNT:
      assert(false);
      break;
    }

  for(int x = 0; x < level->w; x++){
    for (int y = 0 ; y < level->h; y++) {
      uint16_t id = GetCellID(level, x, y);
      RenderTile_World(tileset, id, level, renderer, &gameData->camera, x, y, 1, 1);
    }
  }
}

bool IsEntityBelowOtherEntity(Entity* a, Entity* b){
  return a->y < b->y;
}

void RenderEntities(GameData* data, SDL_Renderer* renderer){
  LevelData* lvl = &data->scenes.gameplay.levels[data->scenes.gameplay.currentLevelIndex];

  Entity** SortedEntities = ALLOC_ARRAY(data->arena_scratch, Entity*, lvl->entityCount);
  for (int i = 0; i < lvl->entityCount; i++) {
    SortedEntities[i] = &lvl->entityBuffer[i];
  }

  std::sort(SortedEntities, SortedEntities + lvl->entityCount, IsEntityBelowOtherEntity);
  for (int i = 0; i < lvl->entityCount; i++) {
    Entity* entity = SortedEntities[i];
    if(entity->active == false){
      continue;
    }
    Sprite* sprite = GetSprite_FromEntityState(entity, data->spriteBuffer);
    if(HasBehaviour(entity, Behaviour::IS_PETRIFIED)){
      sprite = GetSpriteFromID(ENTITY_ID::ROCK, data->spriteBuffer);
    }
    float x_animated = std::lerp(entity->x_prev, entity->x, entity->progress_01);
    float y_animated = std::lerp(entity->y_prev, entity->y, entity->progress_01);
    float dropshadow_y = y_animated;
    if(HasBehaviour(entity, Behaviour::JUMPS) && !HasBehaviour(entity, Behaviour::IS_PUSHING)){
     y_animated -= 0.5 * sinf(entity->progress_01 * 3.14);
    }

    Sprite* dropshadow = &data->spriteBuffer[(int)SPRITE_ID::Dropshadow];

    RenderEntity_OnTile(dropshadow, lvl, renderer, &data->camera, x_animated, dropshadow_y, 1, 0.4, false);
    RenderEntity_OnTile(sprite, lvl, renderer, &data->camera, x_animated, y_animated, 1, 1, entity->facing == Direction::RIGHT);
  }        
}
