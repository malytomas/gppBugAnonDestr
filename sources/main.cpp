
#include "func.h"

void execute(Fnc fnc, void *ptr)
{
	(*fnc)(ptr);
}

void runAaa();
void runBbb();

int main()
{
	runAaa();
	runBbb();
	runAaa();
	runBbb();
}

