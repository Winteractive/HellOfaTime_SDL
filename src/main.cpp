#include <windows.h>
#include <fileapi.h>
#include <cstdio>
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_timer.h"
#include "common.h"
#include "arena.h"
#include "gameState.h"

SDL_Window* window;
SDL_Renderer* renderer;

Uint64 NOW;
Uint64 PREV;

const char* NAME_OF_DLL = "Heartburner_game.dll";
const char* NAME_OF_TEMP_DLL = "Heartburner_temp.dll";

typedef void (*Function_Initialize)(GameData* data);
typedef bool (*Function_HandleEvents)(GameData* data, SDL_Event event);
typedef void (*Function_Update)(GameData* data, float dt);
typedef void (*Function_Draw)(GameData* data, SDL_Renderer* renderer);
typedef void (*Function_OnQuit)(SDL_Renderer* renderer);

const char* NAME_OF_FUNC_INIT = "Initialize";
const char* NAME_OF_FUNC_HANDLE_EVENT = "HandleEvents";
const char* NAME_OF_FUNC_UPDATE = "Update";
const char* NAME_OF_FUNC_DRAW = "Draw";
const char* NAME_OF_FUNC_QUIT = "OnQuit";

struct DLL_INFO{
    HMODULE dll;
    FILETIME timestamp;
    Function_Initialize initialize;
    Function_HandleEvents handleEvents;
    Function_Update update;
    Function_Draw draw;
    Function_OnQuit quit;
};

FILETIME GetTimestamp(){
    WIN32_FIND_DATA data;
    HANDLE handle = FindFirstFile(NAME_OF_DLL, &data);
    FILETIME time_of_last_change = data.ftLastWriteTime;
    FindClose(handle);
    return time_of_last_change;
}

bool LoadDLL(DLL_INFO* info, int depth = 0){
    printf("loading dll");

    if(depth > 20){
        printf("failed to write temp DLL");
        return false;
    }
    bool success = CopyFile(NAME_OF_DLL, NAME_OF_TEMP_DLL, false);

    if(!success){
        Sleep(50);
        return LoadDLL(info, depth + 1);   
    }

    info->dll = LoadLibrary(NAME_OF_TEMP_DLL);

    if(info->dll == nullptr){
        printf("could not load dll");
        return false;
    }

    info->initialize = (Function_Initialize)GetProcAddress(info->dll, NAME_OF_FUNC_INIT);    
    info->handleEvents = (Function_HandleEvents)GetProcAddress(info->dll, NAME_OF_FUNC_HANDLE_EVENT);    
    info->update = (Function_Update)GetProcAddress(info->dll, NAME_OF_FUNC_UPDATE);    
    info->draw = (Function_Draw)GetProcAddress(info->dll, NAME_OF_FUNC_DRAW);    
    info->quit = (Function_OnQuit)GetProcAddress(info->dll, NAME_OF_FUNC_QUIT);    

    info->timestamp = GetTimestamp();

    return true;
}

void UnloadDLL(DLL_INFO* dll){
    FreeLibrary(dll->dll);
    dll->dll = nullptr;
    DeleteFile(NAME_OF_TEMP_DLL);
}

void* AllocateGameMemory(){
    void* blob = malloc(GAME_MEMORY_ALLOWANCE);
    if(blob == nullptr){
        printf("fatal error: could not allocate memory");
        return nullptr;
    }

    printf("memory succesfully allocated");
    return blob;
}

void SDL_Setup(){
    SDL_Init(SDL_INIT_EVENTS);
    window = SDL_CreateWindow("pilot", 650, 400, 0);
    renderer = SDL_CreateRenderer(window, NULL);
}

void CalculateDeltaTime(float& dt){
    NOW = SDL_GetTicksNS();
    dt = NOW - PREV;
    dt = SDL_NS_TO_SECONDS(dt);
    PREV = NOW;
}

void DLL_CheckStatus(DLL_INFO* dll){
    FILETIME timestamp = GetTimestamp();
    bool is_timestamp_changed = CompareFileTime(&dll->timestamp, &timestamp) != 0;
    if(is_timestamp_changed){
        UnloadDLL(dll);
        LoadDLL(dll);
    }
}

int main() {

    void* game_memory = AllocateGameMemory();
    if(game_memory == nullptr){
        return 1;
    }

    Memory::Arena* arena = new Memory::Arena();    
    Memory::Initialize(arena, game_memory, GAME_MEMORY_ALLOWANCE);
    GameData* gameData =  (GameData*)Memory::Allocate(arena, sizeof(GameData));

    DLL_INFO dll;
    bool dll_successfully_loaded = LoadDLL(&dll);

    if(dll_successfully_loaded == false){
        return 2;
    }    

    SDL_Setup();    
    dll.initialize(gameData);
    
    bool running = true;
    float dt;
    while(running){

        DLL_CheckStatus(&dll);        
        
        CalculateDeltaTime(dt);    

        SDL_Event event;
        while(SDL_PollEvent(&event)){
            running = dll.handleEvents(gameData, event);
            if(running == false){
                break;
            }
        }

        dll.update(gameData, dt);
        dll.draw(gameData, renderer);
    }

    dll.quit(renderer);
    SDL_Quit();
    return 0;
    
}
