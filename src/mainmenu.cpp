#include "mainmenu.h"
#include "SDL3/SDL_scancode.h"
#include "arena.h"
#include "button.h"
#include "common.h"
#include "gameState.h"
#include "input.h"
#include "rendering.h"
#include "spriteLibrary.h"
#include <cassert>

 
void InitializeMenu(MainMenu* mainmenu, Sprite* spriteBuffer, Memory::Arena* arena_main){
  assert(mainmenu->initialized == false);
  mainmenu->button_count = 2;
  mainmenu->buttons = ALLOC_ARRAY(arena_main, Button, mainmenu->button_count);
  
  SetupButton(&mainmenu->buttons[0], ButtonType::START_GAME, spriteBuffer, {SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0, 200, 80}, ButtonMode::Centered);  
  SetupButton(&mainmenu->buttons[1], ButtonType::QUIT, spriteBuffer, {SCREEN_WIDTH / 2.0, (SCREEN_HEIGHT / 2.0) + 100, 200, 80}, ButtonMode::Centered);  
  
  mainmenu->initialized = true;
}

void UpdateMenu(GameData* data){
  MainMenu* mainmenu = &data->scenes.mainMenu;
  Input* input = &data->input;
  mainmenu->activeButtonCount = GetActiveButtonCount(mainmenu->buttons, mainmenu->button_count);
  if(mainmenu->activeButtonCount == 0){
    return;
  }
  mainmenu->activeButtons = ALLOC_ARRAY(data->arena_scratch, Button*, mainmenu->activeButtonCount);
  
  int index = 0;
  for (int i = 0; i < mainmenu->button_count; i++) {
    Button* button = &mainmenu->buttons[i];
    if(button->is_active){
      mainmenu->activeButtons[index] = button;
      index += 1;
    }
  }

  int* buttonIndex = &mainmenu->activeButtonIndex;

  bool anyHoveredOver = false;
  bool mouseMoving = input->mouse_magnitude > 0.1;
  if(mouseMoving){
     for (int i = 0; i < mainmenu->activeButtonCount; i++) {
      Button* button = mainmenu->activeButtons[i];
      if(IsHoveredOver(button, input->mouse_x, input->mouse_y)){
        anyHoveredOver = true;
        *buttonIndex = i;
        break;
      }
    } 
  }

  bool up = KeyPressed(input, SDL_SCANCODE_UP);
  bool down = KeyPressed(input, SDL_SCANCODE_DOWN);

  if(up || down){
    int direction = up ? 1 : -1;
    *buttonIndex += direction + mainmenu->activeButtonCount;
    *buttonIndex = *buttonIndex % mainmenu->activeButtonCount;
  }
    
  Button* selected = mainmenu->activeButtons[*buttonIndex];

  if(selected != nullptr){
    if(KeyPressed(input, SDL_SCANCODE_RETURN)){
        PressButton(mainmenu->activeButtons[*buttonIndex], data);
        return;
    }

    if(IsHoveredOver(selected, input->mouse_x, input->mouse_y)){
      if(MousePressed(input, MouseButtons::LEFT)){
        PressButton(selected, data);
        return;
      }
    } 
  }
}

void DrawMenu(MainMenu* mainmenu,  SDL_Renderer* renderer, Sprite* spriteBuffer){
  Sprite* background = GetSprite(SPRITE_ID::titlescreen_background, spriteBuffer);
  float scale = (SCREEN_HEIGHT / ((float)background->height * UPSCALE_FACTOR));
  RenderSprite_World(GetSprite(SPRITE_ID::titlescreen_background, spriteBuffer), renderer, NULL, SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0, scale);
  for (int i = 0; i < mainmenu->activeButtonCount; i++) {
    Button* button = mainmenu->activeButtons[i];
    RenderButton(mainmenu->activeButtons[i], i == mainmenu->activeButtonIndex, renderer);
  }
}
