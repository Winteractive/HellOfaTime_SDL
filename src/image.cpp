#include <cassert>
#include <string>
#include "SDL3/SDL_render.h"
#include "SDL3_Image/SDL_image.h"
#include "image.h"
#include "arena.h"
using namespace std;

const char* DIRECTORY = "assets/sprites/";
const char* FALLBACK = "assets/sprites/fallback.png";

// TODO(MAX) store the SDL_Surface on a scratch-memory-arena instead of letting it
// be allocated on the heap outside our designated block of memory
// IMG_Load allocates for us, would need to swap to IMG_Load_RW and work
// directly with raw file bytes: 
Image* AssetManagement::LoadSprite(Memory::Arena* arena, SDL_Renderer* renderer, const char* name){

  string path = DIRECTORY;
  path = path.append(name);

  SDL_Surface* surface = IMG_Load(path.c_str()); 

  if(surface == nullptr){
    surface = IMG_Load(FALLBACK);
  }

  assert(surface != nullptr);

  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

  Image* img = (Image*)Memory::Allocate(arena, sizeof(Image));
  img->texture = texture;
  img->height = texture->h;
  img->width = texture->w;

  SDL_DestroySurface(surface);

  return img;
}
