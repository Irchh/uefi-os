#include <file.h>
#include <loader.h>

UINTN load_file_efi(CHAR16* file, void** dest) {
	EFI_GUID sfspGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
	EFI_HANDLE* handles = NULL;
	UINTN handleCount = 0;
	EFI_FILE_PROTOCOL* token = NULL;
	UINTN BufferSize = 1024*32;

	EFI_STATUS efiStatus = gBS->LocateHandleBuffer(
									   ByProtocol,
									   &sfspGuid, 
									   NULL, 
									   &handleCount, 
									   &handles);

	for (int index = 0; index < (int)handleCount; ++ index) {
		EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs = NULL;

		efiStatus = gBS->HandleProtocol(
			handles[index],
			&sfspGuid,
			(void**)&fs);

		EFI_FILE_PROTOCOL* root = NULL;
		efiStatus = fs->OpenVolume(fs, &root);

		token = NULL;
		efiStatus = root->Open(
			root, 
			&token,
			file,
			EFI_FILE_MODE_READ,
			EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);
		if (efiStatus == EFI_SUCCESS)
			break;
		else
			continue;
	}
	//efiStatus = token->Read(token, &BufferSize, NULL);
	//void* buf = malloc(1024*32);
	//*dest = malloc(1024*32);
	efiStatus = token->Read(token, &BufferSize, *dest);
	return BufferSize;
}