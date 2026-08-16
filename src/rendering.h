#pragma once
#include "SDL3/SDL_render.h"
#include "camera.h"
#include "fontLibrary.h"
#include "levels.h"
#include "spriteLibrary.h"

struct Button;
struct FontAtlas;

enum class Alignment {
  Right,
  Centered
};

void RenderTile(Sprite* tileset, int cell_id, LevelData* level, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale, float alpha);
void RenderSprite_World(SpriteRenderInfo spriteInfo, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale = 1, float alpha = 1, bool flipped = false);
void RenderSprite_OnTile(SpriteRenderInfo spriteInfo, LevelData* level, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale = 1, float alpha = 1, bool flipped = false);
void RenderButton(Button* button, bool is_selected, SDL_Renderer* renderer);
void RenderText(FontAtlas* atlas, const char* text, SDL_Renderer* renderer, Camera* camera, const float x, const float y, Alignment alignment);
