#include <cstddef>
#include <windows.h>
#include <fileapi.h>
#include <cstdio>
#include <fstream>

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_log.h"
#include "command.h"
#include "common.h"
#include "arena.h"
#include "gameState.h"
#include "image.h"

SDL_Window* window;
SDL_Renderer* renderer;

Uint64 NOW;
Uint64 PREV;

constexpr const char* NAME_OF_DLL = "hellofatime_game.dll";
constexpr const char* NAME_OF_TEMP_DLL = "hellofatime_temp.dll";

typedef void (*Function_Initialize) (GameData* data, SDL_Window* window, SDL_Renderer* renderer);
typedef bool (*Function_HandleEvents) (GameData* data, SDL_Event event);
typedef void (*Function_Update) (GameData* data, float dt);
typedef void (*Function_Draw) (GameData* data, SDL_Renderer* renderer);
typedef void (*Function_OnQuit) (SDL_Renderer* renderer);

constexpr const char* NAME_OF_FUNC_INIT = "Initialize";
constexpr const char* NAME_OF_FUNC_HANDLE_EVENT = "HandleEvents";
constexpr const char* NAME_OF_FUNC_UPDATE = "Update";
constexpr const char* NAME_OF_FUNC_DRAW = "Draw";
constexpr const char* NAME_OF_FUNC_QUIT = "OnQuit";


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
    printf("loading dll \n");
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

void UnloadDLL(DLL_INFO* info){
    FreeLibrary(info->dll);
    info->dll = nullptr;
    DeleteFile(NAME_OF_TEMP_DLL);
}

void* AllocateGameMemory(){
    void* blob = malloc(GAME_MEMORY_ALLOWANCE);
    if(blob == nullptr){
        printf("fatal error: could not allocate memory");
        return nullptr;
    }

    printf("memory succesfully allocated \n");
    return blob;
}

void SDL_Setup(){
    SDL_Init(SDL_INIT_EVENTS);
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);

    window = SDL_CreateWindow("pilot", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, NULL);
}

void CalculateDeltaTime(float* dt){
    NOW = SDL_GetTicksNS();
    *dt = NOW - PREV;
    *dt = SDL_NS_TO_SECONDS(*dt);
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

void CalculateRemainingFrameTime_MS(double* milliseconds){
    Uint64 frame_end_time_ns = SDL_GetTicksNS();
    double frame_time_spent_ns = frame_end_time_ns - PREV;
    double frame_time_spent_ms = frame_time_spent_ns / 1e6;
    *milliseconds = FRAME_TIME_MS - frame_time_spent_ms;
}

void StoreGameState(Memory::Arena* arena){
    std::ofstream file("temp_state.bin", std::ios::binary);
    file.write(reinterpret_cast<const char*>(arena->base), arena->size);
}

void RetrieveGameState(Memory::Arena* arena){
    std::ifstream file("temp_state.bin", std::ios::binary);
    file.read(reinterpret_cast<char*>(arena->base), arena->size);
}

int main() {
    void* game_memory = AllocateGameMemory();
    if(game_memory == nullptr){
        return 1;
    }

    SDL_Setup();    

    
    Memory::Arena* arena_main = new Memory::Arena();    
    Memory::Initialize(arena_main, game_memory, GAME_MEMORY_ALLOWANCE);
    GameData* gameData =  (GameData*)Memory::Allocate(arena_main, sizeof(GameData));

    size_t IMAGE_ARENA_SIZE = sizeof(Image) * 1000;
    gameData->fps_buffer_count = 500;
    gameData->fps_buffer = (float*)Memory::Allocate(arena_main, sizeof(float) * gameData->fps_buffer_count);

    
    gameData->arena_images = Memory::CreateSubArena(arena_main, IMAGE_ARENA_SIZE);
    gameData->arena_levels = Memory::CreateSubArena(arena_main, MEGABYTES(3));
    gameData->arena_entities = Memory::CreateSubArena(gameData->arena_levels, KILOBYTES(32));
    gameData->arena_commands = Memory::CreateSubArena(gameData->arena_levels, MEGABYTES(1));

    gameData->input_buffer_capacity = 50;
    size_t RING_BUFFER_SIZE = sizeof(Position) * gameData->input_buffer_capacity;
    gameData->input_buffer = (Position*)Memory::Allocate(gameData->arena_levels, RING_BUFFER_SIZE);
 
    gameData->levelCount = 500;
    gameData->levels = (LevelData*)Memory::Allocate(gameData->arena_levels, sizeof(LevelData) * gameData->levelCount);
    gameData->keys_previous = (bool*)Memory::Allocate(gameData->arena_levels, sizeof(bool) * SDL_SCANCODE_COUNT);

    gameData->commandBuffer = (CommandBuffer*)Memory::Allocate(arena_main, sizeof(CommandBuffer));
    gameData->commandBuffer->capacity = 20000;
    size_t COMMAND_SIZE = sizeof(AnyCommand) * gameData->commandBuffer->capacity;
    gameData->commandBuffer->allCommands = (AnyCommand*)Memory::Allocate(gameData->arena_commands, COMMAND_SIZE);

    DLL_INFO dll;
    bool dll_successfully_loaded = LoadDLL(&dll);

    if(dll_successfully_loaded == false){
        return 2;
    }    

    dll.initialize(gameData, window, renderer);

    gameData->fallback = AssetManagement::LoadSprite(gameData->arena_images, renderer, "fallback.png");
  
    MMRESULT result = timeBeginPeriod(1);
    if(result == TIMERR_NOCANDO){
        printf("could not increase timer resolution");
        Sleep(2000);
        return 3;
    }
    
    bool running = true;
    float dt;
    gameData->dt = &dt;

    while(running){

        DLL_CheckStatus(&dll);        
        
        CalculateDeltaTime(&dt);    

        SDL_Event event;
        while(SDL_PollEvent(&event)){

            running = dll.handleEvents(gameData, event);
            if(running == false){
                break;
            }

            if(event.type == SDL_EVENT_KEY_DOWN){
                if(event.key.key == SDLK_F9){
                    StoreGameState(arena_main);
                }
               if(event.key.key == SDLK_F10){
                    RetrieveGameState(arena_main);
                }
            }
        }

        dll.update(gameData, dt);
        
        memcpy((void*)gameData->keys_previous, SDL_GetKeyboardState(nullptr), SDL_SCANCODE_COUNT * sizeof(bool));

        dll.draw(gameData, renderer);
 
        double time_to_sleep_ms;
        CalculateRemainingFrameTime_MS(&time_to_sleep_ms);

        if(time_to_sleep_ms > 0){
            if(time_to_sleep_ms > 1){
                SDL_Delay(time_to_sleep_ms - 1);
            }
            while (time_to_sleep_ms > 0) {
                CalculateRemainingFrameTime_MS(&time_to_sleep_ms);        
            }
        }
        else{
            //printf("missed frame \n");
        }
    }

    dll.quit(renderer);
    SDL_Quit();
    return 0;
}
