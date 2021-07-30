#define CAGE_FUNC_EXPORT

#include "func.h"

#include <cstdlib>

MemoryArena &systemMemory()
{
	static MemoryArena mem;
	return mem;
}
