#pragma once
#include "arena.h"
#include "command.h"
#include "entity.h"
#include "image.h"
#include "imgui/imgui_internal.h"
#include "input.h"
#include "levels.h"

struct GameData {
  Image* fallback;
  Image* wall;
  Image* ground;
  Image* player;
  Image* box;
  Memory::Arena* arena_levels;
  Memory::Arena* arena_entities;
  Memory::Arena* arena_images;
  Memory::Arena* arena_commands;
  CommandBuffer* commandBuffer;

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
