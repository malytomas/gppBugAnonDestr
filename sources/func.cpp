#include "func.h"

void execute(Fnc fnc, void *ptr)
{
	(*fnc)(ptr);
}
