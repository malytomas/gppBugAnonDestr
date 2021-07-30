#include <cstdint>
#include <utility>

using uintPtr = std::uintptr_t;

struct MemoryArena;

// delegate

struct Delegate
{
private:
	void (MemoryArena::*fnc)(void*) = nullptr;
	MemoryArena *inst = nullptr;

public:
	template<void(MemoryArena::*F)(void *)>
	Delegate &bind(MemoryArena *i) noexcept
	{
		fnc = F;
		inst = i;
		return *this;
	}

	constexpr void operator ()(void *p) const
	{
		(inst->*fnc)(p);
	}
};

// holder

template<class T> struct Holder;

template<class T>
struct Holder
{
	Holder() noexcept = default;

	Holder(T *data, Delegate deleter) : data_(data), deleter_(deleter)
	{}

	Holder &operator = (Holder &&other) noexcept
	{
		if (this == &other)
			return *this;
		clear();
		std::swap(data_, other.data_);
		std::swap(deleter_, other.deleter_);
		return *this;
	}

	~Holder()
	{
		clear();
	}

	T *operator -> () const
	{
		return data_;
	}

	void clear()
	{
		if (data_)
		{
			deleter_(data_);
			data_ = nullptr;
			deleter_ = {};
		}
	}

protected:
	T *data_ = nullptr;
	Delegate deleter_;
};

// memory arena

struct MemoryArena
{
public:
	template<class T, class... Ts>
	T *createObject(Ts... vs)
	{
		return new T(std::forward<Ts>(vs)...);
	}

	template<class T, class... Ts>
	Holder<T> createHolder(Ts... vs)
	{
		T *ptr = createObject<T>(std::forward<Ts>(vs)...);
		Delegate del;
		del.template bind<&MemoryArena::destroy<T>>(this);
		return Holder<T>(ptr, del);
	};

	template<class T>
	void destroy(void *ptr)
	{
		delete (T *)ptr;
	}
};

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

	int *runTest()
	{
		if (Tester::counter != 0)
			throw "failed sanity check";
		{
			MemoryArena mem;
			{
				Holder<Tester> arr[42] = {};
				for (auto &it : arr)
					it = mem.createHolder<Tester>();
				if (Tester::counter != 42)
					throw "constructors not called";
				for (auto &it : arr)
					it->execute();
			}
		}
		if (Tester::counter != 0)
			throw "destructors not called";
		return &Tester::counter;
	}
}
