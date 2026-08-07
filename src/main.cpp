#include <cmath>
#include <cstddef>
#include <windows.h>
#include <fileapi.h>
#include <cstdio>
#include <fstream>

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_log.h"
#include "command.h"
#include "common.h"
#include "arena.h"
#include "entity.h"
#include "gameState.h"
#include "image.h"
#include "input.h"
#include "spriteLibrary.h"

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

    window = SDL_CreateWindow("hell of a time", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, NULL);
}

void CalculateDeltaTime(float* dt, float scaler){
    NOW = SDL_GetTicksNS();
    *dt = NOW - PREV;
    *dt = SDL_NS_TO_SECONDS(*dt);
    *dt *= scaler;
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
    GameData* gameData = ALLOC(arena_main, GameData);
    gameData->arena_main = arena_main;

    int SPRITE_COUNT = 256;
    size_t IMAGE_ARENA_SIZE = MEGABYTES(1);
    gameData->arena_images = Memory::CreateSubArena(arena_main, IMAGE_ARENA_SIZE);
    gameData->spriteBuffer = ALLOC_ARRAY(gameData->arena_images, Sprite, SPRITE_COUNT);
    gameData->tilesetBuffer = ALLOC_ARRAY(gameData->arena_images, Tileset, (int)TILESETS::COUNT);

    gameData->editor_data.fps_buffer_count = 500;
    gameData->editor_data.fps_buffer = ALLOC_ARRAY(arena_main, float, gameData->editor_data.fps_buffer_count);

    gameData->arena_scratch = Memory::CreateSubArena(arena_main, KILOBYTES(256));

    size_t INPUT_ARENA_SIZE = 0;
    INPUT_ARENA_SIZE += sizeof(bool) * SDL_SCANCODE_COUNT * 2;
    INPUT_ARENA_SIZE += sizeof(float) * SDL_SCANCODE_COUNT * 1;
    INPUT_ARENA_SIZE += 128;
    gameData->arena_input = Memory::CreateSubArena(arena_main, INPUT_ARENA_SIZE);

    gameData->input.keys_current = ALLOC_ARRAY(gameData->arena_input, bool, SDL_SCANCODE_COUNT);
    gameData->input.keys_previous = ALLOC_ARRAY(gameData->arena_input, bool, SDL_SCANCODE_COUNT);
    gameData->input.keys_held_time = ALLOC_ARRAY(gameData->arena_input, float, SDL_SCANCODE_COUNT);
    gameData->input.mouse_held_time = ALLOC_ARRAY(gameData->arena_input, float, (int)MouseButtons::COUNT);

    gameData->arena_levels   = Memory::CreateSubArena(arena_main, MEGABYTES(3));
    gameData->arena_entities = Memory::CreateSubArena(gameData->arena_levels, KILOBYTES(512));
    gameData->arena_commands = Memory::CreateSubArena(gameData->arena_levels, MEGABYTES(1));

    Gameplay* gameplay = &gameData->scenes.gameplay;
    gameplay->input_buffer_capacity = 50;
    gameplay->input_buffer = ALLOC_ARRAY(gameData->arena_levels, Position, gameplay->input_buffer_capacity);
 
    gameplay->levelCount = 500;
    gameplay->levels = ALLOC_ARRAY(gameData->arena_levels, LevelData, gameplay->levelCount);
    gameplay->commandBuffer = ALLOC(arena_main, CommandBuffer);
    gameplay->commandBuffer->capacity = 20000;
    gameplay->commandBuffer->allCommands = ALLOC_ARRAY(gameData->arena_commands, AnyCommand, gameplay->commandBuffer->capacity);

    DLL_INFO dll;
    bool dll_successfully_loaded = LoadDLL(&dll);

    if(dll_successfully_loaded == false){
        return 2;
    }    

    dll.initialize(gameData, window, renderer);
  
    MMRESULT result = timeBeginPeriod(1);
    if(result == TIMERR_NOCANDO){
        printf("could not increase timer resolution");
        Sleep(2000);
        return 3;
    }

    gameData->running = true;
    float dt;
    float dt_scaler = 1;
    gameData->dt = &dt;
    gameData->dt_scaler = &dt_scaler;
    while(gameData->running){

        DLL_CheckStatus(&dll);        

        Reset(gameData->arena_scratch);
        
        CalculateDeltaTime(&dt,  dt_scaler); 

        SDL_Event event;
        while(SDL_PollEvent(&event)){

            gameData->running = dll.handleEvents(gameData, event);
            if(gameData->running == false){
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

        gameData->input.keys_current = SDL_GetKeyboardState(nullptr);
        float* delta_x = &gameData->input.mouse_x_delta;
        float* delta_y = &gameData->input.mouse_y_delta;
        *delta_x = gameData->input.mouse_x; // store last frames value
        *delta_y = gameData->input.mouse_y; // store last frames value
        gameData->input.mouse_current = SDL_GetMouseState(&gameData->input.mouse_x, &gameData->input.mouse_y);
        *delta_x = gameData->input.mouse_x - *delta_x;
        *delta_y = gameData->input.mouse_y - *delta_y;
        float dx = *delta_x;
        float dy = *delta_y;
        gameData->input.mouse_magnitude = std::sqrt(dx * dx + dy * dy);
        // printf("x: %.4f y: %.4f \n", *delta_x, *delta_y);
        // printf("%.4f \n", gameData->input.mouse_magnitude);

        dll.update(gameData, dt);

        UpdateKeys(&gameData->input, dt);
        UpdateMouse(&gameData->input, dt);

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
