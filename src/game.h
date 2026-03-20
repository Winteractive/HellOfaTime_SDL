#include "SDL3/SDL_render.h"
namespace Core{
    void Initialize();
    void Update(float dt);
    void Draw(SDL_Renderer* renderer);
    void OnQuit(SDL_Renderer* renderer);
}
