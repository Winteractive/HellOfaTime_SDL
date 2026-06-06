#pragma once
#include "entity.h"
#include <cstdint>

enum class CMD_TYPE : uint8_t{
  NONE = 0,
  MOVE = 1
};

struct Command {
  CMD_TYPE type;
};

struct MoveCommand : Command{
  Entity* entity;
  int xDir;
  int yDir;
};

union AnyCommand {
  Command command;
  MoveCommand move;
  AnyCommand(MoveCommand mv) : move(mv) {};
};

struct CommandBuffer{
  AnyCommand* allCommands;
  int capacity;
  int index;
};

void Push(CommandBuffer* buffer, AnyCommand cmd);
void Undo(CommandBuffer* buffer);
void Redo(CommandBuffer* buffer);
