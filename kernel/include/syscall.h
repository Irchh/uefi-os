#include <console.h>

struct reg
{
	__int128 xmm15;
	__int128 xmm14;
	__int128 xmm13;
	__int128 xmm12;
	__int128 xmm11;
	__int128 xmm10;
	__int128 xmm9;
	__int128 xmm8;
	__int128 xmm7;
	__int128 xmm6;
	__int128 xmm5;
	__int128 xmm4;
	__int128 xmm3;
	__int128 xmm2;
	__int128 xmm1;
	__int128 xmm0;
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t rbp;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;
};

enum Syscall
{
	sys_read = 0,
	sys_write,
	sys_open,
	sys_close,
};

void syscall(struct reg* regs);