#pragma once
#include "arena.h"
#include "camera.h"
#include "command.h"
#include "entity.h"
#include "imgui/imgui_internal.h"
#include "input.h"
#include "leveleditor.h"
#include "levels.h"
#include "spriteLibrary.h"


enum class SCENE_TYPES : uint8_t{
  NONE,
  TITLESCREEN,
  MAINMENU,
  GAME,
  CREDITS,
};

struct Gameplay {
  CommandBuffer* commandBuffer;
  LevelData* levels;
  int levelCount;
  int currentLevelIndex;
  float undo_timer;
  Position* input_buffer;
  int input_buffer_capacity;
  int input_buffer_write_count;
  int input_buffer_read_count;
  bool initialized;
};

struct MainMenu {
  enum Buttons {
    Play = 0,
    Exit = 1,
    Count = 2
  };
  int button_index;
};

struct Transition {
  enum States {
    Inactive,
    FadeTo,
    FadeFrom
  };
  States state;
  float fade_time_elapsed;
  float fade_time_duration = 1;
};

struct TitleScreen {
};

struct Credits {
  float scrollspeed; 
};

struct Scenes{
  Gameplay gameplay;
  MainMenu mainMenu;
  TitleScreen titlescreen;
  Credits credts;
};

struct EditorData{
  float* fps_buffer;
  int fps_buffer_count;
  bool edit_level;
  Editor editor;
  int fps_buffer_index;
};

struct GameData {
  SCENE_TYPES scene_current;
  SCENE_TYPES scene_previous;
  Scenes scenes;
  Transition transition;
  EditorData editor_data;
  Input input;
  Sprite* spriteBuffer;

  Memory::Arena* arena_main;
  Memory::Arena* arena_levels;
  Memory::Arena* arena_entities;
  Memory::Arena* arena_images;
  Memory::Arena* arena_commands;
  Memory::Arena* arena_input;
  Memory::Arena* arena_scratch;
  Camera camera;
  ImGuiContext* imGui_context;
  const float* dt;  
  
 };

inline LevelData* GetCurrentLevel(Gameplay* game){
    return &game->levels[game->currentLevelIndex];
}
