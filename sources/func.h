#if defined(_MSC_VER)
#define CAGE_API_EXPORT __declspec(dllexport)
#define CAGE_API_IMPORT __declspec(dllimport)
#else
#define CAGE_API_EXPORT [[gnu::visibility("default")]]
#define CAGE_API_IMPORT [[gnu::visibility("default")]]
#endif

#ifdef CAGE_FUNC_EXPORT
#define CAGE_CORE_API CAGE_API_EXPORT
#else
#define CAGE_CORE_API CAGE_API_IMPORT
#endif

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

	template<class D, R(*F)(D, Ts...)>
	Delegate &bind(D d) noexcept
	{
		static_assert(sizeof(d) <= sizeof(inst));
		static_assert(std::is_trivially_copyable_v<D> && std::is_trivially_destructible_v<D>);
		union U
		{
			void *p;
			D d;
		};
		fnc = +[](void *inst, Ts... vs) {
			U u;
			u.p = inst;
			return F(u.d, std::forward<Ts>(vs)...);
		};
		U u;
		u.d = d;
		inst = u.p;
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

	template<class C, R(C:: *F)(Ts...) const>
	Delegate &bind(const C *i) noexcept
	{
		fnc = +[](void *inst, Ts... vs) {
			return (((const C *)inst)->*F)(std::forward<Ts>(vs)...);
		};
		inst = const_cast<C *>(i);
		return *this;
	}

	constexpr explicit operator bool() const noexcept
	{
		return !!fnc;
	}

	constexpr void clear() noexcept
	{
		inst = nullptr;
		fnc = nullptr;
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

	Holder(T *data, Delegate<void(void *)> deleter): data_(data), deleter_(deleter)
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
			deleter_.clear();
		}
	}

protected:
	T *data_ = nullptr;
	Delegate<void(void *)> deleter_;
};

// memory arena

struct OperatorNewTrait {};

inline void *operator new(uintPtr size, void *ptr, OperatorNewTrait) noexcept { return ptr; }
inline void *operator new[](uintPtr size, void *ptr, OperatorNewTrait) noexcept { return ptr; }
inline void operator delete(void *ptr, void *ptr2, OperatorNewTrait) noexcept {}
inline void operator delete[](void *ptr, void *ptr2, OperatorNewTrait) noexcept {}

struct CAGE_CORE_API MemoryArena
{
public:
	void *allocate(uintPtr size, uintPtr alignment);
	void deallocate(void *ptr);

	template<class T, class... Ts>
	T *createObject(Ts... vs)
	{
		void *ptr = allocate(sizeof(T), alignof(T));
		try
		{
			return new(ptr, OperatorNewTrait()) T(std::forward<Ts>(vs)...);
		}
		catch (...)
		{
			deallocate(ptr);
			throw;
		}
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
		if (!ptr)
			return;
		((T *)ptr)->~T();
		deallocate(ptr);
	}
};

CAGE_CORE_API MemoryArena &systemMemory();

