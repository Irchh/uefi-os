#ifndef __string_h__
#define __string_h__

#include <stddef.h>

#if !defined(NULL)
	#define NULL ((void*)0)
#endif

size_t strlen(char* str);
char* strcat(char* str1, char* str2);
void itoa(long long i, char* res, int base);

#endif // __string_h__