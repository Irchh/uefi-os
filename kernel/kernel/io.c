#include <efihelper.h>
#include <io.h>
#include <mem.h>
#include <string.h>

void char_to_wchar(char* str, CHAR16* res, int len) {
	for (int i = 0; i < len; ++i){
		res[i] = str[i];
	}
}

int newlines(char* str, int len) {
	int n = 0;
	for (int i = 0; i < len; ++i)
		if (str[0] == '\n')
			n++;
	return n;
}

void printStrW(CHAR16* str) {
	gST->ConOut->OutputString(gST->ConOut, str);
}

void _printWithBS(char* str);
void (*__printer)(char*) = _printWithBS;

void _printAfter(char* str) {
	// TODO: make output work after exit 
}

void _printWithBS(char* str) {
	if (exit) {
		__printer = _printAfter;
		__printer(str);
	}
	int len = strlen(str);
	int new = newlines(str, len);
	
	CHAR16 res[len+new];

	char_to_wchar(str, res, len);
	gST->ConOut->OutputString(gST->ConOut, res);
}

inline void printStr(char* str) {
	__printer(str);
}

char* vformat(const char* fmt, va_list args) {
	char* str = malloc(sizeof(char)*1);
	str[0] = 0;
	char* backup;

	bool interp_next = true;
	while (*fmt != '\0') {
		if (*fmt == '\\') {
			interp_next = false;
		}
		else if (*fmt == '%' && interp_next)
		{
			fmt++;
			switch(*fmt) {
				case '\0':
					fmt--;
					break;
				case 'i':
					long long int_val = va_arg(args, long long);
					char* intstr = malloc(22*sizeof(char));
					if(!intstr){
						//error(ERR_MEM); // Memory error, most likely out of memory
						printStrW(L"\r\n___PRINTF ERROR___\r\n___MEMORY ERROR___");
						while(1);
					}
					itoa(int_val, intstr, 10);
					backup = str;
					str = strcat(str, intstr);
					free(intstr);
					if (str) {
						free(backup);
					} else {
						//error(ERR_MEM);
						printStrW(L"\r\n___PRINTF ERROR___\r\n___MEMORY ERROR___");
						while(1);
					}
					break;
				case 'x':
					long long hex_val = va_arg(args, long long);
					char* hexstr = malloc(22*sizeof(char));
					if(!hexstr){
						//error(ERR_MEM); // Memory error, most likely out of memory
						printStrW(L"\r\n___PRINTF ERROR___\r\n___MEMORY ERROR___");
						while(1);
					}
					itoa(hex_val, hexstr, 16);
					backup = str;
					str = strcat(str, hexstr);
					free(hexstr);
					if (str) {
						free(backup);
					} else {
						//error(ERR_MEM);
						printStrW(L"\r\n___PRINTF ERROR___\r\n___MEMORY ERROR___");
						while(1);
					}
					break;
				case 's':
					char* str_val = va_arg(args, char*);
					backup = str;
					str = strcat(str, str_val);
					free(str_val);
					if (str) {
						free(backup);
					} else {
						//error(ERR_MEM);
						printStrW(L"\r\n___PRINTF ERROR___\r\n___MEMORY ERROR___");
						while(1);
					}
					break;
				case 'c':
					char char_val = va_arg(args, unsigned int);
					char charstr[2];
					charstr[0] = char_val;
					charstr[1] = 0;
					backup = str;
					str = strcat(str, charstr);
					if (str) {
						free(backup);
					} else {
						//error(ERR_MEM);
						printStrW(L"\r\n___PRINTF ERROR___\r\n___MEMORY ERROR___");
						while(1);
					}
					break;
				default:
					fmt--;
					break;
			}
		}else{
			interp_next = true;
			char test[2];
			test[0] = *fmt;
			test[1] = 0;
			backup = str;
			str = strcat(str, test);
			if (str) {
				free(backup);
			} else {
				//error(ERR_MEM);
				printStrW(L"\r\n___PRINTF ERROR___\r\n___MEMORY ERROR___");
				while(1);
			}
		}
		fmt++;
	}
	return str;
}

char* format(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char* res = vformat(fmt, args);
	va_end(args);
	return res;
}

void vprintf(const char* fmt, va_list args) {
	char* str = vformat(fmt, args);
	printStr(str);
	free(str);
}

void printf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}