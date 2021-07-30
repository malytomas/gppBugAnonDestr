#include <cstdint>
#include <utility>

using uintPtr = std::uintptr_t;

// delegates

template<class T>
struct Delegate;

template<class R, class... Ts>
struct Delegate<R(Ts...)>
{
private:
	R(*fnc)(void *, Ts...) = nullptr;
	void *inst = nullptr;

public:
	template<R(*F)(Ts...)>
	constexpr Delegate &bind() noexcept
	{
		fnc = +[](void *inst, Ts... vs) {
			return F(std::forward<Ts>(vs)...);
		};
		return *this;
	}

	template<class C, R(C:: *F)(Ts...)>
	Delegate &bind(C *i) noexcept
	{
		fnc = +[](void *inst, Ts... vs) {
			return (((C *)inst)->*F)(std::forward<Ts>(vs)...);
		};
		inst = i;
		return *this;
	}

	constexpr R operator ()(Ts... vs) const
	{
		return fnc(inst, std::forward<Ts>(vs)...);
	}
};

// holder

template<class T> struct Holder;

template<class T>
struct Holder
{
	Holder() noexcept = default;

	Holder(T *data, Delegate<void(void *)> deleter) : data_(data), deleter_(deleter)
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
	Delegate<void(void *)> deleter_;
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
		Delegate<void(void *)> del;
		del.template bind<MemoryArena, &MemoryArena::destroy<T>>(this);
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
		if (Tester::counter != 0) // sanity check
			throw;
		{
			MemoryArena mem;
			{
				Holder<Tester> arr[42] = {};
				for (auto &it : arr)
					it = mem.createHolder<Tester>();
				if (Tester::counter != 42) // test constructors
					throw;
				for (auto &it : arr)
					it->execute();
			}
		}
		if (Tester::counter != 0) // test destructors
			throw;
		return &Tester::counter;
	}
}
