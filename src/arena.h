#pragma once

namespace Memory {
  
  struct Arena {
      unsigned char* base;
      size_t size;
      size_t used;  
  };

  void Initialize(Arena* arena, void* memory, size_t size);
  void* Allocate(Arena* arena, size_t size);
  void Reset(Arena* arena); 
  
}


