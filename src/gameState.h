#pragma once
#include "arena.h"
#include "command.h"
#include "image.h"
#include "imgui/imgui_internal.h"
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

  ImGuiContext* imGui_context;
  const float* dt;  
  bool* keys_previous;
  LevelData* levels;
  int levelCount;
  int currentLevelIndex;
  float undo_timer;
  uint32_t command_timestamp;

  LevelData* GetCurrentLevel(){
    return &levels[currentLevelIndex];
  }
};
