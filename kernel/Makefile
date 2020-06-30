# $@ = target file
# $< = first dependency
# $^ = all dependencies

CC = clang
CFLAGS = \
		--target=x86_64-unknown-windows -ffreestanding -Wall -Werror \
		-Wno-unused-variable -Wno-unused-function \
		-fshort-wchar -mno-red-zone -I./include -I /usr/include/efi -I /usr/include/efi/x86_64
LDFLAGS = \
		--target=x86_64-unknown-windows \
		-nostdlib \
		-Wl,-entry:_start \
		-Wl,-subsystem:efi_application \
		-fuse-ld=lld-link

C_SOURCES = $(wildcard cpu/*.c kernel/*.c drivers/*.c)
ASM_SOURCES = $(filter-out cpu/kernel_entry.asm, $(wildcard cpu/*.asm)) $(wildcard cpu/*.S)
HEADERS = $(wildcard cpu/*.h kernel/*.h drivers/*.h)
# Nice syntax for file extension replacement
OBJ1 = ${ASM_SOURCES:.asm=.o} ${C_SOURCES:.c=.o}
OBJ = ${OBJ1:.S=.o}

# First rule is the one executed when no parameters are fed to the Makefile
all: kernel.efi

kernel.efi: ${OBJ}
	${CC} ${LDFLAGS} $^ -o $@

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

%.o: %.S
	${CC} ${CFLAGS} -masm=intel -c $< -o $@

%.o: %.asm
	nasm $< -f win64 -o $@

%.efi: %.o
	${CC} ${LDFLAGS} $< -o $@

test:
	@echo ${OBJ}

clean:
	find . -name "*.o" -exec rm {} \;
	find . -name "*.efi" -exec rm {} \;
	rm -f *.bin *.elf *.o *.dis
