#define CAGE_FUNC_EXPORT

#include "func.h"

#include <cstdlib>

void *MemoryArena::allocate(uintPtr size, uintPtr alignment)
{
	return std::malloc(size);
}

void MemoryArena::deallocate(void *ptr)
{
	std::free(ptr);
}

MemoryArena &systemMemory()
{
	static MemoryArena mem;
	return mem;
}
