#include <mem.h>

void* _UEFI_malloc (size_t size);
void _UEFI_free (void *pool);
void* (*_malloc)(size_t size) = _UEFI_malloc;
void (*_free)(void* pool) = _UEFI_free;

char memArr[1024*4000];
UINTN index = 0;

void memcpy(char *t, const char *s, size_t num) {
	for (int i = 0; i < num; ++i)
	{
		t[i] = s[i];
	}
}

void* _kmalloc (size_t size) {
	index += size+16;
	return &memArr[index-size];
}

void _kfree (void *pool) {
	return;
}

void* _UEFI_malloc (size_t size) {
	if (exit) {
		_malloc = _kmalloc;
		return _malloc(size);
	}
	EFI_STATUS status;
	void *handle;
	status = uefi_call_wrapper(gBS->AllocatePool, 3, EfiBootServicesData, size, &handle);
	if(status == EFI_OUT_OF_RESOURCES || status == EFI_INVALID_PARAMETER)
		return 0;
	else{
		char* arr = handle;
		for (int i = 0; i < size; ++i) arr[i] = 0;
		return handle;
	}
}

void _UEFI_free (void *pool) {
	if (exit) {
		_free = _kfree;
		return _free(pool);
	}
	EFI_STATUS status;
	uefi_call_wrapper(gBS->FreePool, 1, pool);
	gBS->FreePool(pool);
}

void* kmalloc (size_t size) {
	return _malloc(size);
}

void kfree (void *pool) {
	_free(pool);
}

UINT64 bestAllocStart = 0;
UINT64 bestNumberOfPages = 0;
UINT64 nextAllocPage = 0;
void init_mem_after_bs() {
	if (!exit)
		return;
	for (int i = 0; i < mapSize; i += descriptorSize)
	{
		EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)&memoryMap[i];
		if (desc->Type != EfiConventionalMemory) continue;
		if (desc->NumberOfPages > bestNumberOfPages) {
			bestNumberOfPages = desc->NumberOfPages;
			bestAllocStart = desc->PhysicalStart;
		}
	}
	nextAllocPage = bestAllocStart;
}

struct pf_p
{
	uint64_t size;
	uint64_t pf_addr;
	struct pf_p* next;
};

#define MMAP_GET_NUM 0
#define MMAP_GET_ADDR 1
uint64_t mmap_read(uint64_t req, uint64_t mode) {
	if (req == 0) return 0;
	if (mode != MMAP_GET_NUM && mode != MMAP_GET_ADDR) return 0;

	uint64_t cur_mmap_addr = (uint64_t) 1;
	return cur_mmap_addr;
}