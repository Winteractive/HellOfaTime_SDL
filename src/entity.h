#pragma once
#include <cassert>
#include <cstdint>

enum Behaviour : uint32_t {
  NONE = 0,
  CAN_MOVE = 1 << 0,
  IS_PLAYER = 1 << 1,
  RESPOND_TO_INPUT = 1 << 2,
  IS_PETRIFIED = 1 << 3,
};

enum Direction{
  RIGHT,
  LEFT,
  UP,
  DOWN
};

enum class ID : uint8_t {
  NONE = 0,
  GROUND = 1,
  WALL = 2,
  DEMON = 3,
  ROCK = 4,
  MEDUSA = 5,
  GHOST = 6,
  GOLEM = 7,
};

struct Position{
  int x;
  int y;
};

struct Entity{
  ID id;
  Direction facing;
  int strength;
  int x;
  int y;
  int x_prev;
  int y_prev;
  float progress_01;
  Behaviour behaviour;
};

bool IsMoving(Entity* entity);
bool HasBehaviour(Entity* entity, Behaviour flags);
void InitializeBaseBehaviour(Entity* entity);
void SetBehaviour(Entity* entity, Behaviour flags);
void AddBehaviour(Entity* entity, Behaviour flags);
void RemoveBehaviour(Entity* entity, Behaviour flags);
