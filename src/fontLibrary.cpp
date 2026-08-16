#include "fontLibrary.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL_TTF/SDL_ttf.h"
#include <cassert>

namespace AssetManagement{
  void LoadFont(SDL_Renderer* renderer, const char* font_path, FontAtlas* fontAtlas, float ptsize){
    TTF_Font* font = TTF_OpenFont(font_path, ptsize);
    assert(font != nullptr);
    SDL_Color white = {255,255,255,255};

    int atlas_size = 1024;
    SDL_Surface* atlas_surface;
    atlas_surface = SDL_CreateSurface(atlas_size, atlas_size, SDL_PIXELFORMAT_RGBA32);

    int draw_point_x = 0;
    int draw_point_y = 0;
    int tallest_glyph_in_row = 0;

    int FIRST_RELEVANT_GLYPH = 32;
    
    for (int i = FIRST_RELEVANT_GLYPH; i < FontAtlas::GLYPH_COUNT; i++) {
      SDL_Surface* glyph_surface = TTF_RenderGlyph_Blended(font, i, white);

      if(glyph_surface == nullptr){
        continue;
      }

      if(draw_point_x + glyph_surface->w > atlas_size){
        draw_point_x = 0;
        draw_point_y += tallest_glyph_in_row;
        tallest_glyph_in_row = 0;
      }

      if(tallest_glyph_in_row < glyph_surface->h){
        tallest_glyph_in_row = glyph_surface->h;
      }

      SDL_Rect glyph_position = {draw_point_x, draw_point_y, glyph_surface->w, glyph_surface->h};
      SDL_BlitSurface(glyph_surface, NULL, atlas_surface, &glyph_position);

      fontAtlas->glyphs[i].atlasPosition = {(float)glyph_position.x,
                                            (float)glyph_position.y,
                                            (float)glyph_position.w,
                                            (float)glyph_position.h};

      draw_point_x += glyph_surface->w;
          
      SDL_DestroySurface(glyph_surface);
    }
    fontAtlas->atlasTexture = SDL_CreateTextureFromSurface(renderer, atlas_surface);
    SDL_DestroySurface(atlas_surface);
    TTF_CloseFont(font);
  }
}
