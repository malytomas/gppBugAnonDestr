#include "func.h"

int *runAaa();
int *runBbb();

int main()
{
	runAaa();
	runBbb();
	if (runAaa() == runBbb())
		throw; // the two counters may not have same address as their storage is in separate translation units
	runAaa();
	runBbb();
}

