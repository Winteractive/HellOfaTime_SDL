#include "command.h"
#include "entity.h"
#include "levels.h"

enum class FromRedo {No, Yes};

void Execute(AnyCommand cmd, LevelData* level, CommandBuffer* commandBuffer, FromRedo fromRedo = FromRedo::No){
  switch(cmd.command.type){
  case CMD_TYPE::NONE:
    break;
  case CMD_TYPE::MOVE: {
      MoveCommand mv = cmd.move;
      mv.entity->x_prev = mv.entity->x;
      mv.entity->y_prev = mv.entity->y;
      mv.entity->x += mv.xDir;
      mv.entity->y += mv.yDir;
      if(fromRedo == FromRedo::Yes){
        mv.entity->progress_01 = 1;
      }
      mv.entity->action = Actions::MOVING;
      if(fromRedo == FromRedo::No){
        PostMove(mv.entity, level,commandBuffer);
      }
      break;
    }
  case CMD_TYPE::ROTATE:{
      RotateCommand* rotate = &cmd.rotate;
      if(!HasBehaviour(rotate->entity, CAN_ROTATE)){
        break;
      }
      if(fromRedo == FromRedo::Yes){
        rotate->entity->progress_01 = 1;
      }
      rotate->entity->action = Actions::ROTATING;
      if(fromRedo == FromRedo::No){
        PreRotation(rotate->entity, level, commandBuffer, rotate->from, rotate->to);
      }
      rotate->entity->facing_previous = rotate->from;
      rotate->entity->facing_current = rotate->to;
      if(fromRedo == FromRedo::No){
        PostRotation(rotate->entity, level, commandBuffer, rotate->from, rotate->to);
      }
      break;
    }
  case CMD_TYPE::MODIFY_BEHAVIOUR:{
      ModifyBehaviourCommand modify = cmd.modify;
      if(modify.mode == ModifyBehaviourCommand::ADD){
        AddBehaviour(modify.entity, modify.flag); 
      }
      else{
        RemoveBehaviour(modify.entity, modify.flag); 
      }
      break;
    }
    case CMD_TYPE::ADD:{
      AddCommand* add = &cmd.add;
      AddEntity(add->id, add->x, add->y, level);
      break;
    }
    case CMD_TYPE::REMOVE:{
      RemoveCommand* remove = &cmd.remove;
      RemoveEntity(remove->x, remove->y, level);
      break;          
      }
    case CMD_TYPE::SWAP_ACTIVE:{
      SwapActiveEntityCommand* swap = &cmd.swap_active;
      *swap->value_to_change = swap->index_current;
      break;
      }
    }
}

void Push(CommandBuffer* buffer, AnyCommand cmd, LevelData* level){
  assert(cmd.command.type != CMD_TYPE::NONE);
  buffer->allCommands[buffer->index] = cmd;
  buffer->allCommands[buffer->index].command.timestamp = buffer->timestamp;
  buffer->index++;
  buffer->head = buffer->index;
  Execute(cmd, level, buffer, FromRedo::No);
}

void Undo(CommandBuffer* cmdBuffer, LevelData* level){
  if(cmdBuffer->index == 0){
    return;
  }
  cmdBuffer->index--;

  AnyCommand cmd = cmdBuffer->allCommands[cmdBuffer->index];
  assert(cmd.command.type != CMD_TYPE::NONE);
  uint32_t timestamp = cmd.command.timestamp;
  switch(cmd.command.type){
    case CMD_TYPE::NONE:
      break;
    case CMD_TYPE::MOVE:{
      MoveCommand mv = cmd.move;
      mv.entity->x -= mv.xDir;      
      mv.entity->y -= mv.yDir;
      mv.entity->progress_01 = 1;
      }
       break;
    case CMD_TYPE::ROTATE:{
      RotateCommand rotate = cmd.rotate;
      if(!HasBehaviour(rotate.entity, CAN_ROTATE)){
        break;
      }
      rotate.entity->facing_current = rotate.from;
      rotate.entity->progress_01 = 1;
      break;
      }
    case CMD_TYPE::MODIFY_BEHAVIOUR: {
        ModifyBehaviourCommand modify = cmd.modify;
        if(modify.mode == ModifyBehaviourCommand::ADD){
          RemoveBehaviour(modify.entity, modify.flag); 
        }
        else{
          AddBehaviour(modify.entity, modify.flag); 
        }
        break;
      }
      case CMD_TYPE::ADD:{
          AddCommand* add = &cmd.add;
          RemoveEntity(add->x, add->y, level);
          break;          
        }
      case CMD_TYPE::REMOVE:{
          RemoveCommand* remove = &cmd.remove;
          AddEntity(remove->storedID, remove->x, remove->y, level);
          Entity* entity = GetEntity(level, remove->x, remove->y);
          SetBehaviour(entity, remove->storedBehaviour); 
          break;            
        }
      case CMD_TYPE::SWAP_ACTIVE:{
          SwapActiveEntityCommand* swap = &cmd.swap_active;
          *swap->value_to_change = swap->index_previous;
          break;
        }
      }

  if(cmdBuffer->index > 0){
    if(cmdBuffer->allCommands[cmdBuffer->index - 1].command.timestamp == timestamp){
      Undo(cmdBuffer, level);
    } 
  }
}

void Redo(CommandBuffer* cmdBuffer, LevelData* level){
  AnyCommand cmd = cmdBuffer->allCommands[cmdBuffer->index];
    if(cmdBuffer->index == cmdBuffer->head){
    return;
  }
  assert(cmd.command.type != CMD_TYPE::NONE);
  uint32_t timestamp = cmd.command.timestamp;
  Execute(cmd, level, cmdBuffer, FromRedo::Yes);

  cmdBuffer->index++;

  if(cmdBuffer->index != cmdBuffer->head){
    AnyCommand nextCommand = cmdBuffer->allCommands[cmdBuffer->index];
    if(nextCommand.command.timestamp == timestamp){
      Redo(cmdBuffer, level);
    }
  }
}
void ResetCommandBuffer(CommandBuffer* buffer){
  buffer->index = 0;
  buffer->head = 0;
  buffer->timestamp = 0;
}
