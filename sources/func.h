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

template<class T>
inline T *create()
{
	return new T();
}

template<class T>
inline void destroy(void *ptr)
{
	delete (T *)ptr;
}

using Fnc = void (*)(void *);

CAGE_CORE_API void execute(Fnc fnc, void *ptr);
