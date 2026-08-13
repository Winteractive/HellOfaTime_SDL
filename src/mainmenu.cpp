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

  mainmenu->background_horizon = GetSprite(SPRITE_ID::Menu_Horizon, spriteBuffer);
  mainmenu->background_cloud_back = GetSprite(SPRITE_ID::Menu_Cloud_Back, spriteBuffer);
  mainmenu->background_cloud_front = GetSprite(SPRITE_ID::Menu_Cloud_Front, spriteBuffer);
  mainmenu->background_middle = GetSprite(SPRITE_ID::Menu_Middle, spriteBuffer);
  mainmenu->background_front = GetSprite(SPRITE_ID::Menu_Front, spriteBuffer);
  
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

void DrawMenu(MainMenu* mainmenu, SDL_Renderer* renderer, Sprite* spriteBuffer, Input* input){
  float scale = (SCREEN_HEIGHT / ((float)mainmenu->background_horizon->height * UPSCALE_FACTOR));
  scale *= 1.2;
  float mouse_x = input->mouse_x;
  float mouse_y = input->mouse_y;
  float center_x = SCREEN_WIDTH / 2.0;
  float center_y = SCREEN_HEIGHT / 2.0;
  float offset_x = center_x - mouse_x;
  float offset_y = center_y - mouse_y;
  RenderSprite_World(GetSprite(SPRITE_ID::Menu_Horizon, spriteBuffer),     renderer, NULL, center_x, center_y, scale);
  RenderSprite_World(GetSprite(SPRITE_ID::Menu_Cloud_Back, spriteBuffer),  renderer, NULL, center_x + (offset_x / 11), center_y + (offset_y / 11), scale);
  RenderSprite_World(GetSprite(SPRITE_ID::Menu_Cloud_Front, spriteBuffer), renderer, NULL, center_x + (offset_x / 9), center_y + (offset_y / 9), scale);
  RenderSprite_World(GetSprite(SPRITE_ID::Menu_Middle, spriteBuffer),      renderer, NULL, center_x + (offset_x / 7), center_y + (offset_y / 7), scale);
  RenderSprite_World(GetSprite(SPRITE_ID::Menu_Front, spriteBuffer),       renderer, NULL, center_x + (offset_x / 5), center_y + (offset_y / 5), scale);
  for (int i = 0; i < mainmenu->activeButtonCount; i++) {
    Button* button = mainmenu->activeButtons[i];
    RenderButton(mainmenu->activeButtons[i], i == mainmenu->activeButtonIndex, renderer);
  }
}
