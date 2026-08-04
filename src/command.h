#pragma once
#include "entity.h"
#include <cstdint>

enum class CMD_TYPE : uint8_t{
  NONE = 0,
  MOVE = 1,
  ROTATE = 2,
  MODIFY_BEHAVIOUR = 3,
  ADD = 4,
  REMOVE = 5,
  SWAP_ACTIVE = 6
};

struct Command {
  CMD_TYPE type = CMD_TYPE::NONE;
  uint32_t timestamp;
};

struct SwapActiveEntityCommand : Command {
  int index_current;
  int index_previous;
  int* value_to_change;

  SwapActiveEntityCommand(int* activeEntityIndex, int limit){
    index_previous = *activeEntityIndex;
    index_current = *activeEntityIndex + 1;
    value_to_change = activeEntityIndex;
    index_current %= limit;
    type = CMD_TYPE::SWAP_ACTIVE;
  }
};

struct RemoveCommand : Command{
  int x;
  int y;
  Behaviour storedBehaviour;
  ENTITY_ID storedID;

  RemoveCommand(Entity* entity){
    x = entity->x;
    y = entity->y;
    storedBehaviour = entity->behaviour;
    storedID = entity->id;
    type = CMD_TYPE::REMOVE;
  }
};

struct AddCommand : Command{
  int x;
  int y;
  ENTITY_ID id;

  AddCommand(int x, int y, ENTITY_ID id){
    this->x = x;
    this->y = y;
    this->id = id;
    type = CMD_TYPE::ADD;
  }
};

struct ModifyBehaviourCommand : Command {
  enum Mode{
    ADD,
    REMOVE
  };
  
  Entity* entity;
  Behaviour flag;
  Mode mode;
  ModifyBehaviourCommand(Entity* entity, Behaviour flag, Mode mode){
  this->entity = entity;
  this->flag = flag;
  this->mode = mode;
  type = CMD_TYPE::MODIFY_BEHAVIOUR;
  }
};

struct RotateCommand : Command{
  Entity* entity;
  Direction from;
  Direction to;

  RotateCommand(Entity* entity, Direction from, Direction to){
    this->entity = entity;
    this->from = from;
    this->to = to;
    type = CMD_TYPE::ROTATE;
  }
};

struct MoveCommand : Command {
  Entity* entity;
  int xDir;
  int yDir;

  MoveCommand(Entity* entity, int xDir, int yDir){
    this->entity = entity;
    this->xDir = xDir;
    this->yDir = yDir;
    type = CMD_TYPE::MOVE;
  }
};

union AnyCommand {
  Command command;
  MoveCommand move;
  RotateCommand rotate;
  ModifyBehaviourCommand modify;
  AddCommand add;
  RemoveCommand remove;
  SwapActiveEntityCommand swap_active;
  
  AnyCommand(MoveCommand mov){
    move = mov;
  };
  AnyCommand(RotateCommand rot){
    rotate = rot;
  };
  AnyCommand(ModifyBehaviourCommand mod){
    modify = mod;
  }
  AnyCommand(AddCommand add){
    this->add = add;
  }
  AnyCommand(RemoveCommand rem){
    remove = rem;
  }
  AnyCommand(SwapActiveEntityCommand swa){
    swap_active = swa;
  }
};

struct CommandBuffer{
  AnyCommand* allCommands;
  int capacity;
  int index;
  int head;
  uint32_t timestamp;
};

void Push(CommandBuffer* cmdBuffer, AnyCommand cmd, LevelData* level);
void Undo(CommandBuffer* cmdBuffer, LevelData* level);
void Redo(CommandBuffer* cmdBuffer, LevelData* level);
