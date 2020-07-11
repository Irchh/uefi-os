#include <stdio.h>

int main(int argc, char const *argv[])
{
	for (int i = 0; i < 256; ++i)
		printf("extern void interrupt_handler_%i();\n", i);
	printf("\nint main(int argc, char const *argv[]) {\n");
	for (int i = 0; i < 256; ++i)
		printf("\tint_handlers[%i] = (uint64_t)&interrupt_handler_%i;\n", i, i);
	printf("}\n");
	return 0;
}