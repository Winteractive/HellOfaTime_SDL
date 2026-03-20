#include <cstddef>
using namespace std;

namespace Memory {
  
  struct Arena {
      unsigned char* start;
      size_t size;
      size_t offset;  
  };

  void Initialize(Arena* arena, void* memory, size_t size);
  void* Allocate(Arena* arena, size_t size);
  void Reset(Arena* arena); 
  
}


