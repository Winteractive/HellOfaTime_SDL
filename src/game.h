#pragma once

#include "SDL3/SDL_render.h"
#include "gameState.h"

extern "C" {
   __declspec(dllexport) void Initialize(GameData* data, SDL_Window* window, SDL_Renderer* renderer);
   __declspec(dllexport) bool HandleEvents(GameData* data, SDL_Event event);
   __declspec(dllexport) void Draw(GameData* data, SDL_Renderer* renderer);
   __declspec(dllexport) void Update(GameData* data, float dt);
   __declspec(dllexport) void OnQuit(SDL_Renderer* renderer);
   bool TryMove(Entity* mover, LevelData* level,  CommandBuffer* cmd_buffer, int xDir, int yDir, int strength);
   void ChangeScene(GameData* data, SCENE_TYPES new_scene);
}

