
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

void execute(Fnc fnc, void *ptr);
