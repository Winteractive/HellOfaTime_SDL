#pragma once
#include <cassert>
#include <cstdint>

enum Behaviour : uint32_t {
  NONE = 0,
  CAN_MOVE = 1 << 0,
  IS_PLAYER = 1 << 1,
  RESPOND_TO_INPUT = 1 << 2
};

enum class ID : uint8_t {
  NONE = 0,
  GROUND = 1,
  WALL = 2,
  PLAYER = 3,
  BOX = 4
};

struct Position{
  int x;
  int y;
};

struct Entity{
  ID id;
  int x;
  int y;
  int x_prev;
  int y_prev;
  bool is_moving;
  float progress_01;
  Behaviour behaviour;

  bool HasBehaviour(Behaviour flags){
    return (behaviour & flags) == flags;
  }

  void InitializeBaseBehaviour(){
    assert(id != ID::NONE);
    switch (id) {
      default:
        SetBehaviour(NONE);
        break;
      case ID::PLAYER:
        SetBehaviour((Behaviour)(CAN_MOVE | IS_PLAYER | RESPOND_TO_INPUT));
        break;
      case ID::BOX:
        SetBehaviour((Behaviour)CAN_MOVE);
        break;
      }
  }

  
  void SetBehaviour(Behaviour flags){
    behaviour = flags;
  }

  void AddBehaviour(Behaviour flags){
    behaviour = (Behaviour)(behaviour | flags);
  }

  void RemoveBehaviour(Behaviour flags){
    behaviour = (Behaviour)(behaviour & ~flags);
  }
    
};
