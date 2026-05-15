#pragma once
#include <cassert>
#include <cstdint>

enum Behaviour : uint8_t {
  NONE = 0,
  CAN_MOVE = 1 << 0,
  CAN_BE_PUSHED = 1 << 1,
  CAN_PUSH = 1 << 2,
  IS_PLAYER = 1 << 3,
  RESPOND_TO_INPUT = 1 << 4
};

enum class ID : uint8_t {
  GROUND = 1,
  WALL = 2,
  PLAYER = 3
};

struct Entity{
  uint8_t id;
  int x;
  int y;
  Behaviour behaviour;

  bool HasBehaviour(Behaviour flags){
    return (behaviour & flags) == flags;
  }

  void InitializeBaseBehaviour(){
    assert(id != 0);
    switch ((ID)id) {
      default:
        SetBehaviour(NONE);
        break;
      case ID::PLAYER:
        SetBehaviour((Behaviour)(CAN_MOVE | CAN_PUSH | IS_PLAYER | RESPOND_TO_INPUT));
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
