#include <elf.h>
#include <stdint.h>
#include <efi.h>
#include <stdbool.h>
#include <mem.h>

EFI_GUID gEfiLoadedImageProtocolGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
EFI_GUID gEfiSimpleFileSystemProtocolGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
EFI_GUID gEfiFileInfoGuid = EFI_FILE_INFO_ID;

int memcmp(const void *aptr, const void *bptr, size_t n) {
	const unsigned char *a = aptr, *b = bptr;
	for (size_t i = 0; i < n; i++) {
		if (a[i] < b[i]) return -1;
		else if (a[i] > b[i]) return 1;
	}
	return 0;
}

EFI_SYSTEM_TABLE *SystemTable;

void bdgprnt(CHAR16* s) {
	SystemTable->ConOut->OutputString(SystemTable->ConOut, s);
}

EFI_BOOT_SERVICES* gBS;

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *ST)
{
	SystemTable = ST;
	gBS = SystemTable->BootServices;
	bdgprnt(L"Loading kernel.elf...\r\n");
	EFI_FILE *Kernel;
	{
		EFI_HANDLE_PROTOCOL HandleProtocol = SystemTable->BootServices->HandleProtocol;

		EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
		HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (void**)&LoadedImage);

		EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem;
		HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&FileSystem);

		EFI_FILE *Root;
		FileSystem->OpenVolume(FileSystem, &Root);

		EFI_STATUS s = Root->Open(Root, &Kernel, L"EFI\\BOOT\\MyOS\\kernel.elf", EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
		if (s != EFI_SUCCESS) {
			SystemTable->ConOut->OutputString(SystemTable->ConOut, L"kernel is missing\r\n");
			return s;
		}
	}
	bdgprnt(L"Loaded kernel.elf\r\n");

	EFI_ALLOCATE_POOL AllocatePool = SystemTable->BootServices->AllocatePool;

	// load the elf header from the kernel
	bdgprnt(L"Loading elf header...\r\n");
	Elf64_Ehdr header;
	{
		UINTN FileInfoSize;
		EFI_FILE_INFO *FileInfo;
		Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, NULL);
		FileInfo = malloc(FileInfoSize);
		//AllocatePool(EfiLoaderData, FileInfoSize, (void**)&FileInfo);
		Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, (void**)&FileInfo);

		UINTN size = sizeof(header);
		Kernel->Read(Kernel, &size, &header);
	}
	bdgprnt(L"Loaded elf header\r\n");

	// verify the kernel binary
	bdgprnt(L"Verifying...\r\n");
	if (
		memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
		header.e_ident[EI_CLASS] != ELFCLASS64 ||
		header.e_ident[EI_DATA] != ELFDATA2LSB ||
		header.e_type != ET_EXEC ||
		header.e_machine != EM_AMD64 ||
		header.e_version != EV_CURRENT
	) {
		SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Kernel format is bad\r\n");
		return -1;
	}
	bdgprnt(L"Kernel format looks good\r\n");

	// load the kernel segment headers
	bdgprnt(L"Loading ksh...\r\n");
	Elf64_Phdr *phdrs;
	{
		Kernel->SetPosition(Kernel, header.e_phoff);
		UINTN size = header.e_phnum * header.e_phentsize;

		//AllocatePool(EfiLoaderData, size, (void**)&phdrs); // ***CRASH***
		phdrs = malloc(size);
		Kernel->Read(Kernel, &size, phdrs);
	}
	bdgprnt(L"Loaded ksh\r\n");

	EFI_ALLOCATE_PAGES AllocatePages = SystemTable->BootServices->AllocatePages;

	// load the actual kernel binary based on its segment headers
	bdgprnt(L"Load kernel to mem...\r\n");
	for (
		Elf64_Phdr *phdr = phdrs;
		(char*)phdr < (char*)phdrs + header.e_phnum * header.e_phentsize;
		phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize)
		) {
			switch (phdr->p_type) {
			case PT_LOAD: {
				int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000; // round up
				Elf64_Addr segment = phdr->p_paddr;
				AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);

				Kernel->SetPosition(Kernel, phdr->p_offset);
				UINTN size = phdr->p_filesz;
				Kernel->Read(Kernel, &size, (void*)segment);
				break;
			}
			}
		}
	bdgprnt(L"Loaded kernel to memory\r\n");

	bdgprnt(L"Jumping to entry point...\r\n");
	int a = ((int (*)(EFI_HANDLE, EFI_SYSTEM_TABLE*))header.e_entry)(
		ImageHandle, SystemTable
	);
	bdgprnt(L"Kernel returned ");
	switch (a) {
		case 1: { bdgprnt(L"1\r\n"); break;}
		case 2: { bdgprnt(L"2\r\n"); break;}
		case 3: { bdgprnt(L"3\r\n"); break;}
		case 4: { bdgprnt(L"4\r\n"); break;}
		case EFI_SUCCESS: {
			bdgprnt(L"EFI_SUCCESS\r\n"); break;
		}
		default: {
			bdgprnt(L"non-zero value\r\n");
		}
	}

	while(1);
	return EFI_SUCCESS;
}