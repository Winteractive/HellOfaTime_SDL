#pragma once

struct GameData;
struct SDL_Renderer;
struct Button;
struct Sprite;
namespace Memory{
  struct Arena;
}

struct MainMenu {
  Button* buttons;
  int button_count;
  int activeButtonIndex;
  Button** activeButtons;
  int activeButtonCount;
  bool initialized;
};

void InitializeMenu(MainMenu* mainmenu, Sprite* spriteBuffer, Memory::Arena* arena_main);
void UpdateMenu(GameData* data);
void DrawMenu(MainMenu* mainmenu,  SDL_Renderer* renderer, Sprite* spriteBuffer);
