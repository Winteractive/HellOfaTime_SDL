#include "SDL3/SDL_init.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_timer.h"
#include "game.h"
#include "common.h"
#include "arena.h"
#include <cstdio>


SDL_Window* window;
SDL_Renderer* renderer;
Memory::Arena memory;

Uint64 NOW;
Uint64 PREV;

bool HandleRunning(SDL_Event event){
    if(event.type != SDL_EVENT_KEY_DOWN){
        return true;
    }
    if(event.key.key == SDLK_ESCAPE){
        return false;
    }
    else{
        return true;
    }
}

int main() {
        
    SDL_Init(SDL_INIT_EVENTS);
    bool running = true;

    void* blob = malloc(GAME_MEMORY_ALLOWANCE);
    if(blob == nullptr){
        printf("fatal error: could not allocate memory");
        return 1;
    }
    
    printf("memory succesfully allocated");

    Memory::Initialize(&memory, blob, GAME_MEMORY_ALLOWANCE);

    window = SDL_CreateWindow("pilot", 650, 400, 0);
    renderer = SDL_CreateRenderer(window, NULL);

    //Core::Initialize();
    
    while(running){


        NOW = SDL_GetTicksNS();
        float dt = NOW - PREV;
        dt = SDL_NS_TO_SECONDS(dt);
        PREV = NOW;
        
        SDL_Event event;

        while(SDL_PollEvent(&event)){
            running = HandleRunning(event);
        }

        //Core::Update(dt);
        //Core::Draw(renderer);
    }

    //Core::OnQuit(renderer);
    SDL_Quit();
    return 0;
    
}
