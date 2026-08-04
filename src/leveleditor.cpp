#include "leveleditor.h"
#include "command.h"
#include "imgui/imgui.h"
#include "levels.h"
#include "rendering.h"
#include "spriteLibrary.h"

namespace EDITOR {
  void DrawObjectPanel(Editor* editor, Sprite* spriteBuffer){
    ImGui::Begin("objects");
    ImVec2 size = {32, 32};

    if(ImGui::ImageButton("Rock", (ImTextureID)GetSprite(SPRITE_ID::Rock, spriteBuffer)->texture, size)){
     editor->object_to_place_id = ENTITY_ID::ROCK;
    }
    
    ImGui::SameLine();
    if(ImGui::ImageButton("Demon", (ImTextureID)GetSprite(SPRITE_ID::Demon, spriteBuffer)->texture, size)){
     editor->object_to_place_id = ENTITY_ID::DEMON;
    }
    ImGui::SameLine();
    if(ImGui::ImageButton("Medusa", (ImTextureID)GetSprite(SPRITE_ID::Medusa_Rotate, spriteBuffer)->texture, size)){
     editor->object_to_place_id = ENTITY_ID::MEDUSA;
    }

    ImGui::SameLine();
    if(ImGui::ImageButton("Siren", (ImTextureID)GetSprite(SPRITE_ID::Siren, spriteBuffer)->texture, size)){
     editor->object_to_place_id = ENTITY_ID::SIREN;
    }
    ImGui::SameLine();
    
    if(ImGui::ImageButton("Golem", (ImTextureID)GetSprite(SPRITE_ID::Golem, spriteBuffer)->texture, size)){
     editor->object_to_place_id = ENTITY_ID::GOLEM;
    }

    ImGui::End();
  }

  void PlaceObject(const int x, const int y, Editor* editor, LevelData* level, CommandBuffer* commandbuffer){
    AddCommand add(x, y, editor->object_to_place_id);
    Push(commandbuffer, add, level);
  }

  void DrawPreview(Editor* editor, Input* input, SDL_Renderer* renderer, LevelData* level, Camera* camera, Sprite* spriteBuffer){
    int x;
    int y;
    camera::WorldToGrid(input->mouse_x, input->mouse_y, &x, &y, level);
    // Sprite* preview = GetSprite(editor->object_to_place_id, spriteBuffer);
    Sprite* preview = GetSprite(SPRITE_ID::Fallback, spriteBuffer);
    if(preview != nullptr){
      RenderSprite_OnTile(preview, level, renderer, camera, x, y, 1, 0.5);
    }
  }
  void Update(Editor* editor, Input* input, LevelData* level, CommandBuffer* commandBuffer){
    if(MousePressed(input, MouseButtons::LEFT)){
      if(camera::GetIsPointInsideGrid(input->mouse_x, input->mouse_y, level)){
        int x;
        int y;
        camera::WorldToGrid(input->mouse_x, input->mouse_y, &x, &y, level);
        PlaceObject(x, y, editor, level, commandBuffer);
      }
    }
    else if(MousePressed(input, MouseButtons::RIGHT)){
      if(camera::GetIsPointInsideGrid(input->mouse_x, input->mouse_y, level)){
        int x;
        int y;
        camera::WorldToGrid(input->mouse_x, input->mouse_y, &x, &y, level);
        Entity* entity = GetEntity(level, x, y);
        if(entity == nullptr){
          return;
        }
        RemoveCommand remove(entity);
        Push(commandBuffer, remove, level);
       }
    }
  }
}

