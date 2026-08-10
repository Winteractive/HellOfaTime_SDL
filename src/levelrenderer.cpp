#include "levelRenderer.h"
#include "arena.h"
#include "entity.h"
#include "gameState.h"
#include "levels.h"
#include "rendering.h"
#include "spriteLibrary.h"
#include <algorithm>
#include <cmath>

void RenderLevel(GameData* gameData, SDL_Renderer* renderer){
  Gameplay* gameplay = &gameData->scenes.gameplay;
  LevelData* level = &gameplay->levels[gameplay->currentLevelIndex];

  Sprite* sprite;
  switch(level->tileset->type){
    case TILESETS::Dungeon:
      sprite = GetSprite(SPRITE_ID::dungeon_tileset, gameData->spriteBuffer);
      break;
    case TILESETS::NONE:
    case TILESETS::COUNT:
      assert(false);
      break;
    }

  for(int x = 0; x < level->w; x++){
    for (int y = 0 ; y < level->h; y++) {
      uint16_t id = GetCellID(level, x, y);
      RenderTile(sprite, id, level, renderer, &gameData->camera, x, y, 1, 1);
    }
  }

  for(int i = 0; i < level->goalCount; i++){
    Goal goal = level->goals[i];
    Sprite* sprite = GetSprite(SPRITE_ID::Goal, gameData->spriteBuffer);
    int frame = (int)(goal.blink_timer / 0.2) % (sprite->sprite_count_x * sprite->sprite_count_y);
    RenderSprite_OnTile({frame, sprite}, level, renderer, &gameData->camera, goal.x, goal.y);
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

  Gameplay* gameplay = &data->scenes.gameplay;
  Entity* activeEntity = gameplay->activePlayerBuffer[gameplay->activePlayerIndex];
  
  for (int i = 0; i < lvl->entityCount; i++) {
    Entity* entity = SortedEntities[i];
    if(entity->active == false){
      continue;
    }
    SpriteRenderInfo sprite = GetSprite_FromEntityState(entity, data->spriteBuffer);
    if(HasBehaviour(entity, Behaviour::IS_PETRIFIED)){
      sprite = GetSprite(SPRITE_ID::Rock, data->spriteBuffer);
    }
    float x_animated = std::lerp(entity->x_prev, entity->x, entity->progress_01);
    float y_animated = std::lerp(entity->y_prev, entity->y, entity->progress_01);
    float ground_y = y_animated;
    if(entity->action == Actions::MOVING && HasBehaviour(entity, Behaviour::JUMPS) && !HasBehaviour(entity, Behaviour::IS_PUSHING)){
     y_animated -= 0.5 * sinf(entity->progress_01 * 3.14);
    }

    Sprite* dropshadow = &data->spriteBuffer[(int)SPRITE_ID::Dropshadow];

    RenderSprite_OnTile(dropshadow, lvl, renderer, &data->camera, x_animated, ground_y, 1, 0.4, false);
    if(entity == activeEntity){
      SpriteRenderInfo selection_marker = GetSprite(SPRITE_ID::selection_marker, data->spriteBuffer);  
      RenderSprite_OnTile(selection_marker, lvl, renderer, &data->camera, x_animated, ground_y);
    }
    RenderSprite_OnTile(sprite, lvl, renderer, &data->camera, x_animated, y_animated, 1, 1, false);
  }        
}
