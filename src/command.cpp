#include "command.h"

void Execute(AnyCommand cmd, bool from_redo = false){
  switch(cmd.command.type){
  case CMD_TYPE::NONE:
    break;
  case CMD_TYPE::MOVE:
    MoveCommand mv = cmd.move;
    mv.entity->x += mv.xDir;
    mv.entity->y += mv.yDir;
    mv.entity->AddAnimatedPositionToQueue(mv.entity->x, mv.entity->y);
    if(from_redo){
      mv.entity->progress_01 = 1;
      mv.delay = 0;
      mv.entity->SkipToLastAnimation();
    }

    mv.entity->progress_01 -= mv.delay;
    
    break;
  }
}

void Push(CommandBuffer* buffer, AnyCommand cmd, uint32_t timestamp){
  buffer->allCommands[buffer->index] = cmd;
  buffer->allCommands[buffer->index].command.timestamp = timestamp;
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
  uint32_t timestamp = cmd.command.timestamp;
  switch(cmd.command.type){
    case CMD_TYPE::NONE:
      break;
    case CMD_TYPE::MOVE:
      MoveCommand mv = cmd.move;
      mv.entity->x -= mv.xDir;      
      mv.entity->y -= mv.yDir;      
      mv.entity->AddAnimatedPositionToQueue(mv.entity->x, mv.entity->y);
      mv.entity->SkipToLastAnimation();
      mv.entity->progress_01 = 1;
      mv.delay = 0;
      break;
  }

  if(buffer->index > 0){
    if(buffer->allCommands[buffer->index - 1].command.timestamp == timestamp){
      Undo(buffer);
    } 
  }
}

void Redo(CommandBuffer *buffer){
  AnyCommand cmd = buffer->allCommands[buffer->index];
    if(buffer->index == buffer->head){
    return;
  }
  int timestamp = cmd.command.timestamp;
  Execute(cmd, true);

  buffer->index++;

  if(buffer->index != buffer->head){
    AnyCommand nextCommand = buffer->allCommands[buffer->index];
    if(nextCommand.command.timestamp == timestamp){
      Redo(buffer);
    }
  }
}



