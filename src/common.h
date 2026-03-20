#define KILOBYTES(n) ((size_t)n * 1024)
#define MEGABYTES(n) (KILOBYTES(n) * 1024)
#define GIGABYTES(n) (MEGABYTES(n) * 1024)

#define ARENA_ALLOC(arena, type) \
  (reinterpret_cast<type*>(Memory::Allocate((arena), sizeof(type))))

constexpr size_t GAME_MEMORY_ALLOWANCE = MEGABYTES(100);
