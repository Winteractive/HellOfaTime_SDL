#include "arena.h"
#include <cassert>
#include <cstring>


void Memory::Initialize(Arena* arena, void* mem_start, size_t size) {
  arena->base = (unsigned char*)mem_start;  
  arena->size = size;
  arena->used = 0;
}

void* Memory::Allocate(Arena* arena, size_t size) {
  assert(arena->used + size <= arena->size);
  void* front = arena->base + arena->used;
  arena->used += size;
  memset(front, 0, size);
  return front;
}

void Memory::Reset(Arena* arena){
  arena->used = 0;
}

Memory::Arena* Memory::CreateSubArena(Arena* parent_arena, size_t size){
  Memory::Arena* sub_arena = (Memory::Arena*)Allocate(parent_arena, sizeof(Memory::Arena));
  void* memory_start = Allocate(parent_arena, size);
  Memory::Initialize(sub_arena, memory_start, size);
  return sub_arena;
}
