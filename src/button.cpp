#include "button.h"
#include "collision.h"
#include "game.h"
#include "gameState.h"
#include "spriteLibrary.h"

bool IsHoveredOver(Button* button, float x, float y){
  if(button == nullptr) return false;
  if(button->is_active == false) return false;
  assert(button->type != ButtonType::NONE);
  
  return CheckCollisionInsideBounds(button->rect, x, y);
}

void SetupButton(Button* button, ButtonType type, Sprite* spriteBuffer, SDL_FRect rect, ButtonMode mode){
  assert(type != ButtonType::NONE);
  button->type = type;
  button->rect = rect;
  if(mode == ButtonMode::Centered){
    button->rect.x -= button->rect.w / 2;
    button->rect.y -= button->rect.h / 2;
  }
  button->is_active = true;
  switch(button->type){
  case ButtonType::START_GAME:
    button->texture = GetSprite(SPRITE_ID::Fallback, spriteBuffer)->texture;
    break;
  case ButtonType::QUIT:
    button->texture = GetSprite(SPRITE_ID::Fallback, spriteBuffer)->texture;
    break;
  default:
    button->texture = GetSprite(SPRITE_ID::Fallback, spriteBuffer)->texture;
  break;
  }
}

void PressButton(Button *button, GameData *data){
  if(button == nullptr){
    return;
  }
  assert(button->is_active);
  switch(button->type){
  case ButtonType::NONE:
    assert(false);
  case ButtonType::START_GAME:
    ChangeScene(data, SCENE_TYPES::GAME);
    break;
  case ButtonType::QUIT:
    data->running = false;
    break;
  }
}

int GetActiveButtonCount(Button *buttons, int count){
  if(count == 0){
    return 0;
  }
  int amount = 0;
  for (int i = 0; i < count; i++) {
    if(buttons[i].is_active){
      amount++;
    }    
  }
  return amount;
}
