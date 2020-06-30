#include <string.h>
#include <mem.h>

size_t strlen(char* str) {
	int index = 0;
	while (str[index++] != 0);
	return index-1;
}

char* strcat(char* str1, char* str2) {
	size_t str1len = strlen(str1);
	size_t str2len = strlen(str2);

	char* result = malloc((str1len + str2len + 1)*sizeof(char));
	if (!result)
		return 0;

	for (int i = 0; i < str1len; ++i)
		result[i] = str1[i];
	for (int i = str1len; i < str1len+str2len; ++i)
		result[i] = str2[i-str1len];

	result[str1len+str2len+1] = '\0';
	return result;
}

void itoa(long long i, char* res, int base) {
	if (!res) return;
		//res = malloc(22*sizeof(char));

	int k = 0;
	long long j = i;
	while ((j/=base) != 0) k++;

	if (i < 0 && base < 11) {
		k++;
		i *= -1;
		res[0] = '-';
	}

	res[k+1] = 0;
	if (i == 0)
		res[k] = '0';
	while(i != 0) {
		unsigned char n;
		if (i > 0)
			n = i%base;
		else
			n = -i%(base);

		if (n < 0xA)
			res[k] = (char)n + 0x30;
		else
			res[k] = (char)n + 0x37;
		i /= base;
		k--;
	}
}