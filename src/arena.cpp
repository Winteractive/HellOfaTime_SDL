#include "arena.h"
#include <cassert>

void Memory::Initialize(Arena* arena, void* mem_start, size_t size) {
  arena->base = (unsigned char*)mem_start;  
  arena->size = size;
  arena->used = 0;
}

void* Memory::Allocate(Arena *arena, size_t size) {
  assert(arena->used + size < arena->size);
  void* front = arena->base + arena->used;
  arena->used += size;
  return front;
}

void Memory::Reset(Arena *arena){
  arena->used = 0;
}
