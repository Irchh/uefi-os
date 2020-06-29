#include <mem.h>

void* malloc (size_t size) {
	EFI_STATUS status;
	void *handle;
	status = gBS->AllocatePool(EfiBootServicesData, size, &handle);
	if(status == EFI_OUT_OF_RESOURCES || status == EFI_INVALID_PARAMETER)
		return 0;
	else{
		char* arr = handle;
		for (int i = 0; i < size; ++i) arr[i] = 0;
		return handle;
	}
}

void free (void *pool) {
	EFI_STATUS status;
	gBS->FreePool(pool);
}