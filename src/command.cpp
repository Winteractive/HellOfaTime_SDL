#include "command.h"

void Execute(AnyCommand cmd){
  switch(cmd.command.type){
  case CMD_TYPE::NONE:
    break;
  case CMD_TYPE::MOVE:
    MoveCommand mv = cmd.move;
    mv.entity->x += mv.xDir;
    mv.entity->y += mv.yDir;
    break;
  }
}

void Push(CommandBuffer* buffer, AnyCommand cmd){
  buffer->allCommands[buffer->index] = cmd;
  buffer->index++;
  buffer->head = buffer->index;
  Execute(cmd);
}

void Undo(CommandBuffer* buffer){
  if(buffer->index == 0){
    return;
  }
  buffer->index--;

  AnyCommand cmd = buffer->allCommands[buffer->index];
  switch(cmd.command.type){
    case CMD_TYPE::NONE:
      break;
    case CMD_TYPE::MOVE:
      MoveCommand mv = cmd.move;
      mv.entity->x -= mv.xDir;      
      mv.entity->y -= mv.yDir;      
      break;
  }
}

void Redo(CommandBuffer *buffer){
  AnyCommand cmd = buffer->allCommands[buffer->index];
    if(buffer->index == buffer->head){
    return;
  }
  Execute(cmd);
  buffer->index++;
}


