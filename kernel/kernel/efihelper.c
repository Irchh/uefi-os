#include <efihelper.h>
#include <io.h>
#include <mem.h>
#include <console.h>

EFI_HANDLE gIH;
EFI_SYSTEM_TABLE *gST;
EFI_BOOT_SERVICES *gBS;
EFI_RUNTIME_SERVICES *gRS;

EFI_GRAPHICS_OUTPUT_PROTOCOL *gGP = NULL;
UINTN size_of_info;
EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode_info;
EFI_PHYSICAL_ADDRESS lfb_base_addr = 0;

UINTN handleCount;
EFI_HANDLE *handleBuffer;

static EFI_GUID GraphicsOutputProtocolGUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

bool exit = false;
bool init = false;

UINT64 efi_call1(void *func, UINT64 arg1) {
	UINT64 (__attribute__((ms_abi))*f)(UINT64) = func;
	return f(arg1);
}

UINT64 efi_call2(void *func, UINT64 arg1, UINT64 arg2) {
	UINT64 (__attribute__((ms_abi))*f)(UINT64, UINT64) = func;
	return f(arg1, arg2);
}

UINT64 efi_call3(void *func, UINT64 arg1, UINT64 arg2, UINT64 arg3) {
	UINT64 (__attribute__((ms_abi))*f)(UINT64, UINT64, UINT64) = func;
	return f(arg1, arg2, arg3);
}

UINT64 efi_call4(void *func, UINT64 arg1, UINT64 arg2, UINT64 arg3, UINT64 arg4) {
	UINT64 (__attribute__((ms_abi))*f)(UINT64, UINT64, UINT64, UINT64) = func;
	return f(arg1, arg2, arg3, arg4);
}

UINT64 efi_call5(void *func, UINT64 arg1, UINT64 arg2, UINT64 arg3, UINT64 arg4, UINT64 arg5) {
	UINT64 (__attribute__((ms_abi))*f)(UINT64, UINT64, UINT64, UINT64, UINT64) = func;
	return f(arg1, arg2, arg3, arg4, arg5);
}

EFI_STATUS init_efi(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	if (init)
		return EFI_SUCCESS;
	init = true;

	gIH = ImageHandle;
	gST = SystemTable;
	gBS = gST->BootServices;
	gRS = gST->RuntimeServices;

	init_gfx();

	uefi_call_wrapper(gGP->QueryMode, 4, gGP, gGP->Mode->Mode, &size_of_info, &gop_mode_info);
	lfb_base_addr = gGP->Mode->FrameBufferBase;
	InitCon(gop_mode_info);

	return EFI_SUCCESS;
}

void init_gfx() {
	EFI_STATUS status;
	if (gGP == NULL) {
		printf("LocateHandleBuffer\r\n");
		status = uefi_call_wrapper(gBS->LocateHandleBuffer, 5,
						ByProtocol,
						&GraphicsOutputProtocolGUID,
						NULL,
						&handleCount,
						&handleBuffer);
		printf("LocateHandleBuffer end\r\n");
		if (EFI_ERROR(status)) {
			printf("error: LocateHandleBuffer() failed\r\n");
			while(1);
		}
		status = uefi_call_wrapper(gBS->HandleProtocol, 3,
						handleBuffer[0], &GraphicsOutputProtocolGUID, (void**)&gGP);
		if (EFI_ERROR(status) || gGP == NULL) {
			printf("error: HandleProtocol() failed\r\n");
			while(1);
		}
	}

	UINTN mode_num;
	/*
	UINTN size_of_info;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode_info;
	for (mode_num = 0;
		 (status = gGP->QueryMode( gGP, mode_num, &size_of_info, &gop_mode_info )) == EFI_SUCCESS;
		 mode_num++) {
		if (gop_mode_info->HorizontalResolution == 1920 &&
			  gop_mode_info->VerticalResolution == 1080 &&
			  gop_mode_info->PixelFormat        == PixelBlueGreenRedReserved8BitPerColor)
			break;
	}

	if (status != EFI_SUCCESS)//*/
		mode_num = gGP->Mode->Mode;

	status = uefi_call_wrapper(gGP->SetMode, 2, gGP, mode_num);
	if (EFI_ERROR(status)) {
		printf("error: failed to set default mode\r\n");
	}

	printf("Current mode: %i\r\n\r", gGP->Mode->Mode);
}

UINTN load_file_efi(CHAR16* file, void** dest) {
	EFI_GUID sfspGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
	EFI_HANDLE* handles = NULL;
	UINTN handleCount = 0;
	EFI_FILE_PROTOCOL* token = NULL;
	UINTN BufferSize = 1024*32;

	EFI_STATUS efiStatus = uefi_call_wrapper(gBS->LocateHandleBuffer, 5,
									   ByProtocol,
									   &sfspGuid, 
									   NULL, 
									   &handleCount, 
									   &handles);

	for (int index = 0; index < (int)handleCount; ++ index) {
		EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs = NULL;

		efiStatus = uefi_call_wrapper(gBS->HandleProtocol, 3,
			handles[index],
			&sfspGuid,
			(void**)&fs);

		EFI_FILE_PROTOCOL* root = NULL;
		efiStatus = uefi_call_wrapper(fs->OpenVolume, 2, fs, &root);

		token = NULL;
		efiStatus = uefi_call_wrapper(root->Open, 5,
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
	efiStatus = uefi_call_wrapper(token->Read, 3, token, &BufferSize, *dest);
	return BufferSize;
}

void printModes() {
	EFI_STATUS status;
	UINTN mode_num;
	UINTN size_of_info;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode_info;
	for (mode_num = 0;
		 (status = uefi_call_wrapper(gGP->QueryMode, 4, gGP, mode_num, &size_of_info, &gop_mode_info )) == EFI_SUCCESS;
		 mode_num++) {
		printf("Hor: %i, Ver: %i, PF: %i, Mode: %i\t", gop_mode_info->HorizontalResolution,
													   gop_mode_info->VerticalResolution,
													   gop_mode_info->PixelFormat,
													   mode_num);
		if ((mode_num+1)%2 == 0 && mode_num)
			printf("\r\n");
	}
}

UINTN mapSize = 0, mapKey, descriptorSize;
EFI_MEMORY_DESCRIPTOR *memoryMap = NULL;
UINT32 descriptorVersion;
EFI_STATUS exit_services() {
	if (exit)
		return EFI_SUCCESS;
	exit = true;
	EFI_STATUS result;
	result = uefi_call_wrapper(gBS->GetMemoryMap, 5, &mapSize, memoryMap, NULL, &descriptorSize, NULL);
	if (result != EFI_BUFFER_TOO_SMALL) {
		return result;
		//printf("result: %i\n", result);
	}
	
	mapSize += 2 * descriptorSize;
	result = uefi_call_wrapper(gBS->AllocatePool, 4, EfiLoaderData, mapSize, (void **)&memoryMap);
	if (result != EFI_SUCCESS) {
		return result;
		//printf("-- ERROR: AllocatePool --\r\n");
	}

	result = uefi_call_wrapper(gBS->GetMemoryMap, 5, &mapSize, memoryMap, &mapKey, &descriptorSize, &descriptorVersion);
	if (result != EFI_SUCCESS) {
		return result;
		//printf("-- ERROR: GetMemoryMap --\r\n");
	}

	result = uefi_call_wrapper(gBS->ExitBootServices, 2, gIH, mapKey);
	if (result != EFI_SUCCESS && result != EFI_INVALID_PARAMETER) {
		return result;
		// printf("-- ERROR: ExitBootServices --\r\n");
	}
	return EFI_SUCCESS;
}