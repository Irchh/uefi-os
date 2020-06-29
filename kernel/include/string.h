#ifndef __string_h__
#define __string_h__

#if __SIZEOF_POINTER__ == 8
	typedef unsigned long long size_t;
#else
	typedef unsigned int size_t;
#endif

#if !defined(NULL)
	#define NULL ((void*)0)
#endif

size_t strlen(char* str);
char* strcat(char* str1, char* str2);
void itoa(long long i, char* res, int base);

#endif // __string_h__