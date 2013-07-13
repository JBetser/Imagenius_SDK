#pragma once
//converting C-style allocator to c++ style

#ifdef _LIB
#ifdef realloc
#undef realloc
#endif
#ifdef malloc
#undef malloc
#endif
#ifdef calloc
#undef calloc
#endif
#ifdef free
#undef free
#endif

#define malloc(size)\
	new BYTE [size]
#define realloc(reallocmem, mem, size)\
	BYTE *tmp = new BYTE [size];\
	::memcpy_s (tmp, size, mem, sizeof(mem));\
	delete [] mem;\
	reallocmem = tmp;

void* calloc(size_t szType, int nNum);

#define free(mem)\
	delete [] mem

#endif