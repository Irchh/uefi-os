#include <loader.h>
#include <exit.h>

UINTN mapSize = 0, mapKey, descriptorSize;
EFI_MEMORY_DESCRIPTOR *memoryMap = NULL;
UINT32 descriptorVersion;

EFI_STATUS exit_services() {
	EFI_STATUS result;
	result = gBS->GetMemoryMap(&mapSize, memoryMap, NULL, &descriptorSize, NULL);
	if (result != EFI_BUFFER_TOO_SMALL) {
		bdgprnt(L"Error");
		//printf("result: %i\n", result);
		return result;
	}
	
	mapSize += 2 * descriptorSize;
	result = gBS->AllocatePool(EfiLoaderData, mapSize, (void **)&memoryMap);
	if (result != EFI_SUCCESS) {
		bdgprnt(L"-- ERROR: AllocatePool --\r\n");
		return result;
	}

	result = gBS->GetMemoryMap(&mapSize, memoryMap, &mapKey, &descriptorSize, &descriptorVersion);
	if (result != EFI_SUCCESS) {
		bdgprnt(L"-- ERROR: GetMemoryMap --\r\n");
		return result;
	}

	result = gBS->ExitBootServices(gIH, mapKey);
	if (result != EFI_SUCCESS && result != EFI_INVALID_PARAMETER) {
		bdgprnt(L"-- ERROR: ExitBootServices --\r\n");
		return result;
	}
	return EFI_SUCCESS;
}