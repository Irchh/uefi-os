CC = clang
CFLAGS = \
		--target=x86_64-unknown-windows -ffreestanding \
		-fshort-wchar -mno-red-zone -I /usr/include/efi -I /usr/include/efi/x86_64

LD = clang
LDFLAGS = \
		--target=x86_64-unknown-windows \
		-nostdlib \
		-Wl,-entry:efi_main \
		-Wl,-subsystem:efi_application \
		-fuse-ld=lld-link

C_SOURCES = $(wildcard *.c)
OBJ = ${C_SOURCES:.c=.o}

.PHONY: boot kernel boot/boot.bin kernel/kernel.bin os-image.img

all: kernel

boot/boot.bin: boot
kernel/kernel.efi: kernel

kernel:
	cd kernel && ${MAKE}

os-image.img: kernel/kernel.efi
	dd if=/dev/zero of=os-image.img bs=1k count=1440
	mformat -i os-image.img -f 1440 ::
	mmd -i os-image.img ::/EFI
	mmd -i os-image.img ::/EFI/BOOT
	mmd -i os-image.img ::/EFI/BOOT/MyOS
	cp $< BOOTX64.EFI
	mcopy -i os-image.img BOOTX64.EFI ::/EFI/BOOT
	mcopy -i os-image.img font/UbuntuMono-R-8x16.psf ::/EFI/BOOT/MyOS
	rm BOOTX64.EFI

hdimage.bin: os-image.img
	mkgpt -o hdimage.bin --image-size 4096 --part $< --type system

run: os-image.img
	qemu-system-x86_64 -L firmware/ -bios OVMF_CODE.fd -drive format=raw,file=$<
runwin: hdimage.bin
	powershell.exe F:\\qemu\\qemu-system-x86_64.exe -hda X:\\home\\irch\\src\\uefi-os\\$< \
	-L X:\\home\\irch\\src\\uefi-os\\firmware\\ -bios OVMF_CODE.fd

clean:
	rm -rf *.bin *.iso *.o
	cd kernel && ${MAKE} clean
