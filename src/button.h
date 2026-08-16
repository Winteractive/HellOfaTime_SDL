#pragma once
#include "SDL3/SDL_rect.h"
#include "fontLibrary.h"
#include "rendering.h"

struct Sprite;
struct GameData;

enum class ButtonType {
  NONE,
  START_GAME,
  QUIT
};

struct Button {
  ButtonType type;
  SDL_FRect rect;
  Sprite* sprite;
  bool is_active;
  bool is_dynamic;
  FontAtlas* font;
  const char* text;
};


void PressButton(Button* button, GameData* data);
int GetActiveButtonCount(Button* buttons, int count);
bool IsHoveredOver(Button* button, float x, float y);

void SetupButton(Button* button,
                 ButtonType type,
                 Sprite* spriteBuffer,
                 SDL_FRect rect,
                 Alignment mode,
                 FontAtlas* font = nullptr,
                 const char* text = nullptr);  
