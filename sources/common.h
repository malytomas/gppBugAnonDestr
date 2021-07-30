
#include "func.h"

// ANONYMOUS! namespace
namespace
{
	struct Tester
	{
		static inline int counter = 0;

		Tester()
		{
			counter++;
		}

		~Tester()
		{
			counter--;
		}

		Tester(const Tester &) = delete;
		Tester(Tester &&) = delete;
		Tester &operator = (const Tester &) = delete;
		Tester &operator = (Tester &&) = delete;

		void execute()
		{
			// do some meaningless work
			volatile int item = 0;
			for (int i = 0; i < 100; i++)
				item++;
		}
	};

	void runTest()
	{
		if (Tester::counter != 0) // sanity check
			throw;
		{
			Tester *arr[42] = {};
			for (auto &it : arr)
				it = create<Tester>();
			if (Tester::counter != 42) // test constructors
				throw;
			for (auto &it : arr)
				it->execute();
			for (auto &it : arr)
				execute(&destroy<Tester>, it);
		}
		if (Tester::counter != 0) // test destructors
			throw;
	}
}
