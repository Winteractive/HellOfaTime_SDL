#include "leveleditor.h"
#include "imgui/imgui.h"
#include "rendering.h"
#include "spriteLibrary.h"

namespace EDITOR {
  void DrawObjectPanel(Editor* editor, Sprite* spriteBuffer){
    ImGui::Begin("objects");
    ImVec2 size = {32, 32};

    if(ImGui::ImageButton("Ground", (ImTextureID)GetSpriteFromID(ID::GROUND, spriteBuffer)->texture, size)){
      editor->object_to_place_id = ID::GROUND;
    }
    ImGui::SameLine();
    if(ImGui::ImageButton("Wall", (ImTextureID)GetSpriteFromID(ID::WALL, spriteBuffer)->texture, size)){
     editor->object_to_place_id = ID::WALL;
    }
    
    ImGui::SameLine();
    if(ImGui::ImageButton("Rock", (ImTextureID)GetSpriteFromID(ID::ROCK, spriteBuffer)->texture, size)){
     editor->object_to_place_id = ID::ROCK;
    }
    
    ImGui::SameLine();
    if(ImGui::ImageButton("Demon", (ImTextureID)GetSpriteFromID(ID::DEMON, spriteBuffer)->texture, size)){
     editor->object_to_place_id = ID::DEMON;
    }
    ImGui::SameLine();
    if(ImGui::ImageButton("Medusa", (ImTextureID)GetSpriteFromID(ID::MEDUSA, spriteBuffer)->texture, size)){
     editor->object_to_place_id = ID::MEDUSA;
    }

    ImGui::End();
  }

  void PlaceObject(const int x, const int y, Editor* editor, LevelData* level){
    if(editor->object_to_place_id == ID::GROUND || editor->object_to_place_id == ID::WALL){
      level->cells[y * level->w + x] = (int)editor->object_to_place_id;
    }
    else{
      AddEntity(editor->object_to_place_id, x, y, level);
    }
  }

  void DrawPreview(Editor* editor, Input* input, SDL_Renderer* renderer, LevelData* level, Camera* camera, Sprite* spriteBuffer){
    int x;
    int y;
    camera::WorldToGrid(input->mouse_x, input->mouse_y, &x, &y, level);
    Sprite* preview = GetSpriteFromID(editor->object_to_place_id, spriteBuffer);
    if(preview != nullptr){
      RenderSprite_Grid(preview, level, renderer, camera, x, y, 1, 0.5);
    }
  }

  void Update(Editor* editor, Input* input, LevelData* level){
    if(MousePressed(input, MouseButtons::LEFT)){
      if(camera::GetIsPointInsideGrid(input->mouse_x, input->mouse_y, level)){
        int x;
        int y;
        camera::WorldToGrid(input->mouse_x, input->mouse_y, &x, &y, level);
        PlaceObject(x, y, editor, level);
      }
    }
    else if(MousePressed(input, MouseButtons::RIGHT)){
      if(camera::GetIsPointInsideGrid(input->mouse_x, input->mouse_y, level)){
        int x;
        int y;
        camera::WorldToGrid(input->mouse_x, input->mouse_y, &x, &y, level);
        RemoveEntity(x, y, level);
      }
    }
  }
}

