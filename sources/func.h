
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
