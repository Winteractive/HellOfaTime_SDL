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

struct GameData {

  Sprite* spriteBuffer;

  Memory::Arena* arena_main;
  Memory::Arena* arena_levels;
  Memory::Arena* arena_entities;
  Memory::Arena* arena_images;
  Memory::Arena* arena_commands;
  Memory::Arena* arena_input;
  CommandBuffer* commandBuffer;

  Camera camera;
  bool edit_level;
  Editor editorData;
  Input input;

  ImGuiContext* imGui_context;
  const float* dt;  
  LevelData* levels;
  int levelCount;
  int currentLevelIndex;
  float undo_timer;
  uint32_t command_timestamp;
  
  Position* input_buffer;
  int input_buffer_capacity;
  int input_buffer_write_count;
  int input_buffer_read_count;

  float* fps_buffer;
  int fps_buffer_count;
  int fps_buffer_index;

  LevelData* GetCurrentLevel(){
    return &levels[currentLevelIndex];
  }
   
};
