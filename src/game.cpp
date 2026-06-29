#include "game.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "common.h"
#include "dev_gui.h"
#include "command.h"
#include "entity.h"
#include "entityrenderer.h"
#include "gameState.h"
#include "imgui/imgui.h"
#include "levels.h"
#include "image.h"
#include "levelRenderer.h"


bool KeyPressed(SDL_Scancode key, const bool* current, const bool* previous){
  if(previous == nullptr){
    return current[key];
  }
  return current[key] && !previous[key];
}
bool KeyHeld(SDL_Scancode key, const bool* current, const bool* previous){
  if(previous == nullptr){
    return false;
  }
  return current[key] && previous[key];
}
bool KeyReleased(SDL_Scancode key, const bool* current, const bool* previous){
  if(previous == nullptr){
    return false;
  }
  return !current[key] && previous[key];
}

extern "C" {

  void Initialize(GameData* data, SDL_Window* window, SDL_Renderer* renderer){
    
    DEV::Initialize(window, renderer);
    data->imGui_context = ImGui::GetCurrentContext();
    
    data->ground = AssetManagement::LoadSprite(data->arena_images, renderer, "ground.png");
    data->wall   = AssetManagement::LoadSprite(data->arena_images, renderer, "wall.png"); 
    data->player = AssetManagement::LoadSprite(data->arena_images, renderer, "player.png");
    data->box = AssetManagement::LoadSprite(data->arena_images, renderer, "box.png");

    data->currentLevelIndex = 1;
    CreateLevel(data->arena_levels, &data->levels[0], "assets/levels/testLevel.tmj");
    CreateLevel(data->arena_levels, &data->levels[1], "assets/levels/testLevel_box.tmj");
    CreateEntities(&data->levels[data->currentLevelIndex], data->arena_entities);
  }
   
   bool HandleEvents(GameData *data, SDL_Event event){
    DEV::ProcessEvents(&event);

    if(event.type != SDL_EVENT_KEY_DOWN){
        return true;
    }
    if(event.key.key == SDLK_ESCAPE){
        return false;
    }

     return true;
  } 

  void Update(GameData* data,float dt){

    const bool* keys = SDL_GetKeyboardState(nullptr);

    if(KeyPressed(SDL_SCANCODE_R, keys, data->keys_previous)){
      CreateEntities(data->GetCurrentLevel(), data->arena_entities);      
    }

    if(KeyPressed(SDL_SCANCODE_Z, keys, data->keys_previous)){
      if(KeyHeld(SDL_SCANCODE_LSHIFT, keys, data->keys_previous)){
        Redo(data->commandBuffer);
      }
      else{
        Undo(data->commandBuffer);
      }
    }

    data->command_timestamp += 1;

    
    if(KeyPressed(SDL_SCANCODE_RIGHT, keys, data->keys_previous)){
      data->input_buffer[data->input_buffer_write_count++ % data->input_buffer_capacity] = {1, 0};
    }
    else if(KeyPressed(SDL_SCANCODE_LEFT, keys, data->keys_previous)){
      data->input_buffer[data->input_buffer_write_count++ % data->input_buffer_capacity] = {-1, 0};
    }
    else if(KeyPressed(SDL_SCANCODE_UP, keys, data->keys_previous)){
      data->input_buffer[data->input_buffer_write_count++ % data->input_buffer_capacity] = {0, -1};
    }
    else if(KeyPressed(SDL_SCANCODE_DOWN, keys, data->keys_previous)){
      data->input_buffer[data->input_buffer_write_count++ % data->input_buffer_capacity] = {0, 1};
    }

    bool are_entities_moving = false;
    for (int i = 0; i < data->GetCurrentLevel()->entityCount; i++) {
      Entity* entity = &data->GetCurrentLevel()->entityBuffer[i];
      if(entity->HasBehaviour(CAN_MOVE) && entity->is_moving){
        entity->progress_01 += MOVE_SPEED * dt;
        if(entity->progress_01 >= 1){
          entity->progress_01 = 0;
          entity->x_prev = entity->x;
          entity->y_prev = entity->y;
          entity->is_moving = false;
        }
        if(entity->progress_01 > 0 && entity->progress_01 < 1){
          are_entities_moving = true;
        }
      }
    }

    
    if(are_entities_moving == false){
      if(data->input_buffer_read_count == data->input_buffer_write_count){
        return;
      }

      data->command_timestamp += 1;

      for (int i = 0; i < data->GetCurrentLevel()->entityCount; i++) {
        Entity* entity = &data->GetCurrentLevel()->entityBuffer[i];
        if(entity->HasBehaviour((Behaviour)(RESPOND_TO_INPUT | CAN_MOVE))){
          int xDir = data->input_buffer[data->input_buffer_read_count % data->input_buffer_capacity].x;
          int yDir = data->input_buffer[data->input_buffer_read_count % data->input_buffer_capacity].y;
          TryMove(entity, data->GetCurrentLevel(), data->commandBuffer, xDir, yDir, data->command_timestamp);
        }
      }  
      data->input_buffer_read_count++;
    }
  }
 
  
  bool TryMove(Entity* mover, LevelData* level, CommandBuffer* cmd_buffer, int xDir, int yDir, int timestamp){
    if(mover->HasBehaviour(CAN_MOVE) == false){
      return false;
    }

    int test_x = mover->x + xDir;
    int test_y = mover->y + yDir;
    Entity* stepInto_entity = level->GetEntity(test_x, test_y);
    ID stepInto_tile_id = (ID)level->GetCellID(test_x, test_y);
    if(stepInto_entity == nullptr){
      if(stepInto_tile_id == ID::GROUND){
        MoveCommand mv;
        mv.type = CMD_TYPE::MOVE;
        mv.entity = mover;
        mv.xDir = xDir;
        mv.yDir = yDir;
        Push(cmd_buffer, mv, timestamp);
        return true;
      }
      return false;
    }
      
    if(stepInto_entity->HasBehaviour(CAN_MOVE)){
      if(TryMove(stepInto_entity, level, cmd_buffer, xDir, yDir, timestamp)){
        MoveCommand mv;
        mv.type = CMD_TYPE::MOVE;
        mv.entity = mover;
        mv.xDir = xDir;
        mv.yDir = yDir;
        Push(cmd_buffer, mv, timestamp);
        return true;
      }
    }

    return false;
  }
    
  void Draw(GameData* data, SDL_Renderer* renderer){
    DEV::PreDraw(data->imGui_context);
    SDL_SetRenderDrawColor(renderer, 120, 70, 120, 255);
    SDL_RenderClear(renderer);
    
    RenderLevel(data, renderer);  
    RenderEntities(data, renderer);

    DEV::Draw(data, renderer);
    SDL_RenderPresent(renderer);
    
  }

  void OnQuit(SDL_Renderer* renderer){
    SDL_DestroyRenderer(renderer);
  }

}
