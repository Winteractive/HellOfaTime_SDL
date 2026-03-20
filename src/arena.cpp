#include "arena.h"

void Memory::Initialize(Arena* arena, void* mem_start, size_t size) {
  arena->start = (unsigned char*)mem_start;  
  arena->size = size;
  arena->offset = 0;
}

void* Memory::Allocate(Arena *arena, size_t size) {
  void* front = arena->start + arena->offset;
  arena->offset += size;
  return front;
}

void Memory::Reset(Arena *arena){
  arena->offset = 0;
}
