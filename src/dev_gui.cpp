#include "dev_gui.h"
#include "SDL3/SDL_video.h"
#include "common.h"
#include "gameState.h"
#include "imgui/imgui_impl_sdlrenderer3.h"
#include "SDL3/SDL_render.h"
#include "imgui/imgui_internal.h"
#include <string>

using namespace std;

void DEV::Initialize(SDL_Window* window, SDL_Renderer* renderer){
  ImGui::CreateContext();
  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);
  ImGuiStyle* style = &ImGui::GetStyle();
  style->ScaleAllSizes(2.0);
  ImGuiIO& io = ImGui::GetIO();
  int w, h;
  SDL_GetWindowSize(window, &w, &h);
  io.DisplaySize = ImVec2((float)w, (float)h);
}

void DEV::ProcessEvents(SDL_Event* event){
  ImGui_ImplSDL3_ProcessEvent(event);
}
void DEV::PreDraw(ImGuiContext* saved_context){
  if(ImGui::GetCurrentContext() == nullptr){
    ImGui::SetCurrentContext(saved_context);
  }
  
  ImGui::NewFrame();
}

void Draw_Imgui_Arena_Usage(Arena* arena, string name_of_arena){
  float fraction = (float)arena->used / (float)arena->size;
  string barText = name_of_arena;
  barText += " " + to_string(arena->used);
  barText += " / " + to_string(arena->size);
  ImGui::ProgressBar(fraction, ImVec2(-1,0), barText.c_str());
}

void DrawFPS(GameData* data){
  data->fps_buffer[data->fps_buffer_index++] = 1.0 / *data->dt;
  data->fps_buffer_index %= data->fps_buffer_count;
  ImGui::PlotHistogram("fps", data->fps_buffer, data->fps_buffer_count,0,nullptr ,0,FPS, ImVec2(-1,35));
}

void Draw_History(CommandBuffer* buffer, LevelData* level){
  int sliderPos = buffer->index;

  if(ImGui::SliderInt("history",&sliderPos, 0, buffer->head)){
    while(buffer->index > sliderPos){
      Undo(buffer);
    }
    while(buffer->index < sliderPos){
      Redo(buffer, level);
    }
  }
}

void DEV::Draw(GameData* data, SDL_Renderer* renderer){
  ImGui::Begin("Dev Tools");
  ImGui::Text("memory arena usage amount");
  
  Draw_Imgui_Arena_Usage(data->arena_main, "all memory");
  Draw_Imgui_Arena_Usage(data->arena_images, "images");
  Draw_Imgui_Arena_Usage(data->arena_levels, "levels");
  Draw_Imgui_Arena_Usage(data->arena_commands, "commands");
  Draw_Imgui_Arena_Usage(data->arena_entities, "entities");
  Draw_Imgui_Arena_Usage(data->arena_input, "input");
 
  Draw_History(data->commandBuffer, data->GetCurrentLevel());
  DrawFPS(data);
  
  ImGui::End();

  if(data->edit_level){
    EDITOR::DrawObjectPanel(&data->editorData, data->spriteBuffer);
    EDITOR::DrawPreview(&data->editorData, &data->input, renderer, data->GetCurrentLevel(), &data->camera, data->spriteBuffer);
   }

  ImGui::Render();
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer );
}



