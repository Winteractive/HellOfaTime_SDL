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

    if(ImGui::ImageButton("Rock", (ImTextureID)GetSpriteFromID(ENTITY_ID::ROCK, spriteBuffer)->texture, size)){
     editor->object_to_place_id = ENTITY_ID::ROCK;
    }
    
    ImGui::SameLine();
    if(ImGui::ImageButton("Demon", (ImTextureID)GetSpriteFromID(ENTITY_ID::DEMON, spriteBuffer)->texture, size)){
     editor->object_to_place_id = ENTITY_ID::DEMON;
    }
    ImGui::SameLine();
    if(ImGui::ImageButton("Medusa", (ImTextureID)GetSpriteFromID(ENTITY_ID::MEDUSA, spriteBuffer)->texture, size)){
     editor->object_to_place_id = ENTITY_ID::MEDUSA;
    }

    ImGui::SameLine();
    if(ImGui::ImageButton("Siren", (ImTextureID)GetSpriteFromID(ENTITY_ID::SIREN, spriteBuffer)->texture, size)){
     editor->object_to_place_id = ENTITY_ID::SIREN;
    }
    ImGui::SameLine();
    
    if(ImGui::ImageButton("Golem", (ImTextureID)GetSpriteFromID(ENTITY_ID::GOLEM, spriteBuffer)->texture, size)){
     editor->object_to_place_id = ENTITY_ID::GOLEM;
    }

    ImGui::End();
  }

  void PlaceObject(const int x, const int y, Editor* editor, LevelData* level, CommandBuffer* commandBuffer){
    AddCommand add(x, y, editor->object_to_place_id);
    Push(commandBuffer, add, level);
  }

  void DrawPreview(Editor* editor, Input* input, SDL_Renderer* renderer, LevelData* level, Camera* camera, Sprite* spriteBuffer){
    int x;
    int y;
    camera::WorldToGrid(input->mouse_x, input->mouse_y, &x, &y, level);
    Sprite* preview = GetSpriteFromID(editor->object_to_place_id, spriteBuffer);
    if(preview != nullptr){
      RenderEntity_OnTile(preview, level, renderer, camera, x, y, 1, 0.5);
    }
  }

  void Update(Editor* editor, Input* input, LevelData* level, CommandBuffer* buffer){
    if(MousePressed(input, MouseButtons::LEFT)){
      if(camera::GetIsPointInsideGrid(input->mouse_x, input->mouse_y, level)){
        int x;
        int y;
        camera::WorldToGrid(input->mouse_x, input->mouse_y, &x, &y, level);
        PlaceObject(x, y, editor, level, buffer);
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
        Push(buffer, remove, level);
      
      }
    }
  }
}

