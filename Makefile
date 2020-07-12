.PHONY: kernel loader kernel/kernel.elf os-image.img

all: loader kernel

kernel/kernel.elf: kernel
loader/loader.efi: loader

kernel:
	cd kernel && ${MAKE}

loader:
	cd loader && ${MAKE}

os-image.img: loader kernel
	dd if=/dev/zero of=os-image.img bs=1k count=1440
	mformat -i os-image.img -f 1440 ::
	mmd -i os-image.img ::/EFI
	mmd -i os-image.img ::/EFI/BOOT
	mmd -i os-image.img ::/EFI/BOOT/MyOS
	cp loader/loader.efi BOOTX64.EFI
	mcopy -i os-image.img BOOTX64.EFI ::/EFI/BOOT
	mcopy -i os-image.img font/UbuntuMono-R-8x16.psf ::/EFI/BOOT/MyOS
	mcopy -i os-image.img kernel/kernel.elf ::/EFI/BOOT/MyOS
	rm BOOTX64.EFI

hdimage.bin: os-image.img
	mkgpt -o hdimage.bin --image-size 4096 --part $< --type system

run: os-image.img
	qemu-system-x86_64 -L firmware/ -bios OVMF_CODE.fd -drive format=raw,file=$< -d int,cpu_reset
runwin: hdimage.bin
	powershell.exe F:\\qemu\\qemu-system-x86_64.exe -hda X:\\home\\irch\\src\\uefi-os\\$< \
	-L X:\\home\\irch\\src\\uefi-os\\firmware\\ -bios OVMF_CODE.fd -d cpu_reset

rundbg: os-image.img
	qemu-system-x86_64 -L firmware/ -bios OVMF_CODE.fd -drive format=raw,file=$< -d int,cpu_reset -S -s

runwindbg: hdimage.bin
	powershell.exe F:\\qemu\\qemu-system-x86_64.exe -hda X:\\home\\irch\\src\\uefi-os\\$< \
	-L X:\\home\\irch\\src\\uefi-os\\firmware\\ -bios OVMF_CODE.fd -d cpu_reset -S -s

clean:
	rm -rf *.bin *.iso *.o
	cd kernel && ${MAKE} clean
	cd loader && ${MAKE} clean