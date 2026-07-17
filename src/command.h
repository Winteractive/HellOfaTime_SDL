#pragma once
#include "entity.h"
#include <cstdint>

enum class CMD_TYPE : uint8_t{
  NONE = 0,
  MOVE = 1,
  ROTATE = 2,
  MODIFY_BEHAVIOUR = 3,
};

struct Command {
  CMD_TYPE type = CMD_TYPE::NONE;
  uint32_t timestamp;
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

  AnyCommand(MoveCommand mov){
    move = mov;
  };
  AnyCommand(RotateCommand rot){
    rotate = rot;
  };
  AnyCommand(ModifyBehaviourCommand mod){
    modify = mod;
  }
};

struct CommandBuffer{
  AnyCommand* allCommands;
  int capacity;
  int index;
  int head;
  uint32_t timestamp;
};

void Push(CommandBuffer* buffer, AnyCommand cmd, LevelData* level);
void Undo(CommandBuffer* buffer);
void Redo(CommandBuffer* buffer, LevelData* level);
