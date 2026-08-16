#pragma once

#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"

struct Glyph{
  SDL_FRect atlasPosition;
};

struct FontAtlas {
  SDL_Texture* atlasTexture;
  static const int GLYPH_COUNT = 128;
  Glyph glyphs[GLYPH_COUNT];
  
};

namespace AssetManagement{
  void LoadFont(SDL_Renderer* renderer, const char* font_path, FontAtlas* fontAtlas, float ptsize);
}
