#include <syscall.h>

void syscall(struct reg* regs) {
	switch (regs->rax) {
		case sys_write: {
			for (uint64_t i = 0; i < regs->rdx; i++)
				PrintChar(((char*)regs->rsi)[i]);
			break;
		}
		default: {
			PrintStr("Syscall: ");
			PrintInt(regs->rax, 10);
			while(1);
		}
	}
	// Change return value so that we dont get an infinite syscall loop
	((uint64_t*)&regs->rax)[3] += 2;
}