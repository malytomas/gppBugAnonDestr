#include <cstdio>

int *runAaa();
int *runBbb();

int main()
{
	try
	{
		runAaa();
		runBbb();
		if (runAaa() == runBbb())
			throw "counters share same storage"; // the two counters may not have same address as their storage is in anonymous namespaces in separate translation units
		runAaa();
		runBbb();
		printf("ok\n");
	}
	catch (const char *str)
	{
		printf("%s\n", str);
		throw;
	}
}

