#include "game.h"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "arena.h"
#include "common.h"
#include "dev_gui.h"
#include "command.h"
#include "entity.h"
#include "entityrenderer.h"
#include "gameState.h"
#include "imgui/imgui.h"
#include "input.h"
#include "leveleditor.h"
#include "levels.h"
#include "mainmenu.h"
#include "rendering.h"
#include "spriteLibrary.h"
#include "levelRenderer.h"
#include "tilesetLibrary.h"
#include <cmath>

extern "C" {

  void InitializeGame(Gameplay* gameplay, Arena* arena_levels, Tileset* tilesetBuffer){
    assert(gameplay->initialized == false);
    gameplay->currentLevelIndex = 0;
    CreateLevel(arena_levels, &gameplay->levels[0], &tilesetBuffer[(int)TILESETS::Dungeon], "assets/levels/testing.tmj");
    gameplay->initialized = true;
  }

  void Initialize(GameData* data, SDL_Window* window, SDL_Renderer* renderer){
    DEV::Initialize(window, renderer);
    AssetManagement::LoadAllSprites(data->spriteBuffer, renderer);
    data->imGui_context = ImGui::GetCurrentContext();

    AssetManagement::LoadAllTilesets(data->tilesetBuffer, data->arena_images);

    SDL_Texture* blackfade = GetSprite(SPRITE_ID::black_1x1, data->spriteBuffer)->texture;
    SDL_SetTextureBlendMode(blackfade, SDL_BLENDMODE_BLEND);
    InitializeGame(&data->scenes.gameplay, data->arena_levels, data->tilesetBuffer);
    InitializeMenu(&data->scenes.mainMenu, data->spriteBuffer, data->arena_main);
    ChangeScene(data, SCENE_TYPES::MAINMENU);
  }

  

  void StartLevel(Gameplay* gameplay, Arena* arena_commands, Arena* arena_entities){
    Reset(arena_commands);
    CreateEntities(&gameplay->levels[gameplay->currentLevelIndex], arena_entities);
    gameplay->activePlayerIndex = 0;
  }
    
  void ChangeScene(GameData* data, SCENE_TYPES new_scene){
    assert(new_scene != data->scene_current);
    data->scene_previous = data->scene_current;
    data->scene_current = new_scene;
    data->transition.state = data->scene_previous == SCENE_TYPES::NONE ? Transition::FadeFrom : Transition::FadeTo;
    data->transition.fade_time_elapsed = 0;
    switch (data->scene_current) {
      case SCENE_TYPES::TITLESCREEN:
        data->transition.fade_time_duration = 1;
        break;          
      case SCENE_TYPES::MAINMENU:
        
        break;
      case SCENE_TYPES::GAME:{
        data->transition.fade_time_duration = 0.5f;
        Gameplay* gameplay = &data->scenes.gameplay;
        assert(gameplay->initialized);
        StartLevel(gameplay, data->arena_commands, data->arena_entities);
       break;
      }
      case SCENE_TYPES::CREDITS:
       break;
      case SCENE_TYPES::NONE:
        assert(false);
        break;
      }
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

  void UpdateTitlescreen(TitleScreen* titlescreen, const float dt){
  }

  void UpdateGame(Gameplay* gameplay, Input* input, Arena* arena_scratch, const float dt){
    float undo_speed_up = std::lerp(1.0, 0.15, (gameplay->commandBuffer->head - gameplay->commandBuffer->index) * (1.0/30.0));
    if(undo_speed_up < 0.15){
      undo_speed_up = 0.15;
    }
    if(KeyPressed(input, SDL_SCANCODE_Z) || KeyHeld_ForTime(input, SDL_SCANCODE_Z, UNDO_REPEAT_TIME * undo_speed_up)){
      ResetKeyHeldTime(input, SDL_SCANCODE_Z);
      if(KeyHeld(input, SDL_SCANCODE_LSHIFT)){
        Redo(gameplay->commandBuffer, GetCurrentLevel(gameplay));
      }
      else{
        Undo(gameplay->commandBuffer, GetCurrentLevel(gameplay));
      }
    }
   
    if(KeyPressed(input,SDL_SCANCODE_RIGHT) || KeyHeld_ForTime(input,SDL_SCANCODE_RIGHT, (1 / MOVE_SPEED) * 1.15)){
      ResetKeyHeldTime(input, SDL_SCANCODE_RIGHT);
      gameplay->input_buffer[gameplay->input_buffer_write_count++ % gameplay->input_buffer_capacity] = {1, 0};
    }
    else if(KeyPressed(input,SDL_SCANCODE_LEFT) || KeyHeld_ForTime(input,SDL_SCANCODE_LEFT, (1 / MOVE_SPEED) * 1.15)){
      ResetKeyHeldTime(input, SDL_SCANCODE_LEFT);
      gameplay->input_buffer[gameplay->input_buffer_write_count++ % gameplay->input_buffer_capacity] = {-1, 0};
    }
    else if(KeyPressed(input,SDL_SCANCODE_UP) || KeyHeld_ForTime(input,SDL_SCANCODE_UP, (1 / MOVE_SPEED) * 1.15)){
      ResetKeyHeldTime(input, SDL_SCANCODE_UP);
      gameplay->input_buffer[gameplay->input_buffer_write_count++ % gameplay->input_buffer_capacity] = {0, -1};
    }
    else if(KeyPressed(input,SDL_SCANCODE_DOWN) || KeyHeld_ForTime(input,SDL_SCANCODE_DOWN, (1 / MOVE_SPEED) * 1.15)){
      ResetKeyHeldTime(input, SDL_SCANCODE_DOWN);
      gameplay->input_buffer[gameplay->input_buffer_write_count++ % gameplay->input_buffer_capacity] = {0, 1};
    }
    
    
    bool are_entities_acting = false;
    LevelData *level = GetCurrentLevel(gameplay);
    Entity* entityBuffer = level->entityBuffer;

    for (int i = 0; i < level->entityCount; i++){
      if(IsActing(&entityBuffer[i])){
        are_entities_acting = true;
        break;
      }
    }

    for (int i = 0; i < level->entityCount; i++){
      Entity* entity = &entityBuffer[i];
      if(!entity->active) continue;
        switch(entity->action){
        case Actions::NONE:
          continue;
        case Actions::MOVING:
          entity->progress_01 += MOVE_SPEED * dt;
            break;
        case Actions::ROTATING:
          entity->progress_01 += 8 * dt;
          break;
      }
    }
    for (int i = 0; i < level->entityCount; i++){
      Entity* entity = &entityBuffer[i];
      if(entity->progress_01 >= 1){
        entity->x_prev = entity->x;
        entity->y_prev = entity->y;
        entity->facing_previous = entity->facing_current;
        entity->action = Actions::NONE;
        entity->progress_01 = 0;
        if(HasBehaviour(entity, Behaviour::IS_PUSHING)){
          RemoveBehaviour(entity, Behaviour::IS_PUSHING);
        }
      }
    }
    
    int player_count = 0;
    for (int i = 0; i < level->entityCount; i++) {
      if(entityBuffer[i].active == false){
        continue;
      }
      if(HasBehaviour(&level->entityBuffer[i], (Behaviour)(IS_PLAYER))){
        player_count++;
      }
    }

    int index = 0;
    gameplay->activePlayerBuffer = ALLOC_ARRAY(arena_scratch, Entity*, player_count);
    for (int i = 0; i < level->entityCount; i++) {
      if(entityBuffer[i].active == false){
        continue;
      }
      if(HasBehaviour(&level->entityBuffer[i], (Behaviour)(IS_PLAYER))){
        gameplay->activePlayerBuffer[index++] = &level->entityBuffer[i];
      }
    }

    if(are_entities_acting == false && KeyPressed(input, SDL_SCANCODE_X) && player_count > 0){
      SwapActiveEntityCommand swap(&gameplay->activePlayerIndex, player_count);
      Push(gameplay->commandBuffer, swap, GetCurrentLevel(gameplay));
      gameplay->commandBuffer->timestamp += 1;
    }
        
    if(are_entities_acting){
      return;
    }
    if(gameplay->input_buffer_read_count == gameplay->input_buffer_write_count){
      return;
    }

    Entity* entity = GetActiveEntity(gameplay);
    
    if(!HasBehaviour(entity, (Behaviour)(RESPOND_TO_INPUT | CAN_MOVE))){
      return;
    }
    
    if(HasBehaviour(entity, Behaviour::IS_PETRIFIED)){
      return;
    }
    int xDir = gameplay->input_buffer[gameplay->input_buffer_read_count % gameplay->input_buffer_capacity].x;
    int yDir = gameplay->input_buffer[gameplay->input_buffer_read_count % gameplay->input_buffer_capacity].y;

    Direction new_facing = DirectionFromXY(xDir, yDir);
    if(new_facing != entity->facing_current){ 
      RotateCommand rotate(entity, entity->facing_current, new_facing);
      Push(gameplay->commandBuffer, rotate, level);
      return;
    }

    if(!IsActing(entity)){
      TryMove(entity, level, gameplay->commandBuffer, xDir, yDir, entity->strength);
      gameplay->commandBuffer->timestamp += 1;
      gameplay->input_buffer_read_count++;
    }
  }  
    
  

  void Update(GameData* data,float dt){
    TitleScreen* titlescreen = &data->scenes.titlescreen;
    Gameplay* gameplay = &data->scenes.gameplay;
    EditorData* editorData = &data->editor_data;
    Transition* transition = &data->transition;

    if(KeyPressed(&data->input, SDL_SCANCODE_F2)){
      editorData->edit_level = !editorData->edit_level;
    }
    if(editorData->edit_level){
      EDITOR::Update(&editorData->editor, &data->input, GetCurrentLevel(gameplay), gameplay->commandBuffer);
    }
    if(KeyPressed(&data->input, SDL_SCANCODE_5)){
      ChangeScene(data, SCENE_TYPES::TITLESCREEN);
      return;
    }
    
    if(transition->state != Transition::Inactive){
      transition->fade_time_elapsed += dt;
      if(transition->fade_time_elapsed >= transition->fade_time_duration){
        transition->fade_time_elapsed = 0;
        switch (transition->state) {
        case Transition::Inactive:
          break;
        case Transition::FadeTo:
          transition->state = Transition::FadeFrom;
          break;
        case Transition::FadeFrom:
          transition->state = Transition::Inactive;
          break;
        }
      }
    }

    switch(data->scene_current){
    case SCENE_TYPES::TITLESCREEN:
      UpdateTitlescreen(titlescreen, dt);
      if(AnyKeyPressed(&data->input)){
        if(transition->state == Transition::FadeTo || transition->state == Transition::Inactive){
          ChangeScene(data, SCENE_TYPES::GAME);
        }
      }
      break;
    case SCENE_TYPES::MAINMENU:
      UpdateMenu(data);
      break;
    case SCENE_TYPES::GAME:
      UpdateGame(gameplay, &data->input, data->arena_scratch, dt);
      break;
    case SCENE_TYPES::CREDITS:
      break;
    case SCENE_TYPES::NONE:
      assert(false);
      break;
    }
  }
 
  
  bool TryMove(Entity* mover, LevelData* level,CommandBuffer* commandBuffer, int xDir, int yDir, int strength){
    if(HasBehaviour(mover, CAN_MOVE) == false){
      return false;
    }

    if(strength < 0){
      return false;
    }

    int test_x = mover->x + xDir;
    int test_y = mover->y + yDir;
    Entity* stepInto_entity = GetEntity(level, test_x, test_y);
    uint16_t stepInto_tile_id = GetCellID(level, test_x, test_y);
    if(stepInto_entity == nullptr){
      if(IsWalkable(test_x, test_y, level)){
        MoveCommand mv(mover, xDir, yDir);
        Push(commandBuffer, mv, level);
        return true;
      }
      return false;
    }
      
    if(HasBehaviour(stepInto_entity, CAN_MOVE) && !HasBehaviour(stepInto_entity, UNPUSHABLE)){
      if(TryMove(stepInto_entity, level,commandBuffer, xDir, yDir, --strength)){
        MoveCommand mv(mover, xDir, yDir);
        AddBehaviour(mover, Behaviour::IS_PUSHING);
        Push(commandBuffer, mv, level);
        return true;
      }
    }

    return false;
  }

  void DrawScene(GameData* data, SCENE_TYPES scene, SDL_Renderer* renderer){
    switch(scene){
        case SCENE_TYPES::TITLESCREEN:
        RenderSprite_World(data->scenes.titlescreen.background, renderer, &data->camera, 0, 0);
        break;
      case SCENE_TYPES::MAINMENU:
        DrawMenu(&data->scenes.mainMenu, renderer, data->spriteBuffer);
        break;
      case SCENE_TYPES::GAME:
        RenderLevel(data, renderer);  
        RenderEntities(data, renderer);
        break;
      case SCENE_TYPES::CREDITS:
        break;
      case SCENE_TYPES::NONE:
        assert(false);
        break;
      }
  }
      
  void Draw(GameData* data, SDL_Renderer* renderer){
    DEV::PreDraw(data->imGui_context);
    SDL_SetRenderDrawColor(renderer, 120, 70, 120, 255);
    SDL_RenderClear(renderer);

    switch(data->transition.state){
    case Transition::Inactive:
      DrawScene(data, data->scene_current, renderer);
      break;
    case Transition::FadeTo: {
        DrawScene(data, data->scene_previous, renderer);
        float alpha = data->transition.fade_time_elapsed / data->transition.fade_time_duration;
        RenderSprite_World(GetSprite(SPRITE_ID::black_1x1, data->spriteBuffer), renderer, &data->camera, 0, 0, SCREEN_WIDTH,alpha);
      break;
      }
    case Transition::FadeFrom:{
        DrawScene(data, data->scene_current, renderer);
        float alpha = 1 - data->transition.fade_time_elapsed / data->transition.fade_time_duration;
        RenderSprite_World(GetSprite(SPRITE_ID::black_1x1, data->spriteBuffer), renderer, &data->camera, 0, 0, SCREEN_WIDTH,alpha);
      break;
      }
    }
    
    DEV::Draw(data, renderer);
    SDL_RenderPresent(renderer);
  }

  void OnQuit(SDL_Renderer* renderer){
    SDL_DestroyRenderer(renderer);
  }

}
