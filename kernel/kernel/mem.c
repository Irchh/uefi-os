#include <mem.h>
#include <console.h>

void* _kmalloc (size_t size);
void _kfree (void *pool);
void* (*_malloc)(size_t size) = _kmalloc;
void (*_free)(void* pool) = _kfree;

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

void* kmalloc (size_t size) {
	return _malloc(size);
}

void kfree (void *pool) {
	_free(pool);
}

const char *memory_types[] = 
{
	"EfiReservedMemoryType",
	"EfiLoaderCode",
	"EfiLoaderData",
	"EfiBootServicesCode",
	"EfiBootServicesData",
	"EfiRuntimeServicesCode",
	"EfiRuntimeServicesData",
	"EfiConventionalMemory",
	"EfiUnusableMemory",
	"EfiACPIReclaimMemory",
	"EfiACPIMemoryNVS",
	"EfiMemoryMappedIO",
	"EfiMemoryMappedIOPortSpace",
	"EfiPalCode",
};

uint64_t bestAllocStart = 0;
uint64_t bestNumberOfPages = 0;
uint64_t nextAllocPage = 0;
void init_mem_after_bs(uint64_t pages) {
	kmsg(NORET INFO "Init mem\r");

	for (int i = 0; i < sysInfo->mapSize; i += sysInfo->descriptorSize)
	{
		EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint8_t*)sysInfo->memoryMap + i);
		if (desc->Type != EfiConventionalMemory) continue;
		if (desc->NumberOfPages > bestNumberOfPages) {
			bestNumberOfPages = desc->NumberOfPages;
			bestAllocStart = desc->PhysicalStart;
			if (bestNumberOfPages >= pages)
				goto end;
		}
	}
end:
	nextAllocPage = bestAllocStart;
	kmsg(DONE "Init mem");
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

uint8_t* pageframe_dir = 0;
uint64_t pageframe_dir_size = 0;
int setup_pfa(uint64_t pages) {
	kmsg(NORET INFO "Pageframe allocator setup\r");
	if (bestNumberOfPages*0x200 < pages) {
		kmsg(ERR "Not enough space for pageframe directory");
		PrintStr("bestNumberOfPages*0x200 < pages == ");
		PrintInt(bestNumberOfPages*0x200, 16); PrintStr(" < ");
		PrintInt(pages, 16); PrintChar('\n');
		return 0;
	}
	pageframe_dir = (void*)nextAllocPage;
	pageframe_dir_size = pages/8+(pages%8 != 0);

	for (int i = 0; i < pageframe_dir_size; ++i)
		((uint8_t*)pageframe_dir)[i] = 0;

	kmsg(NORET DONE "Pageframe allocator setup. ");
	PrintStr("Pages taken: ");
	PrintInt(pageframe_dir_size/0x1000+(pageframe_dir_size%0x1000 != 0), 10);
	PrintChar('\n');

	return 1;
}

uint64_t first_free_frame() {
	for (int i = 0; i < pageframe_dir_size; ++i)
	{
		if (pageframe_dir[i] == 0xFF)
			continue;
		for (int j = (i == 0); j < 8; ++j)
		{
			if (pageframe_dir[i]&(1<<j))
				continue;
			return i*8+j;
		}
	}
	return 0;
}

uint64_t pageframe_alloc(uint64_t frame) {
	uint64_t byte = frame/8;
	uint8_t bit = 1<<(frame%8);

	if (byte > pageframe_dir_size)
		return 0;

	if ((pageframe_dir[byte]&bit) == 0) {
		pageframe_dir[byte] |= bit;
		return frame;
	}
	return 0;
}

void pageframe_dealloc(uint64_t frame) {
	uint64_t byte = frame/8;
	uint8_t bit = 1<<(frame%8);

	if (byte > pageframe_dir_size)
		return;

	if ((pageframe_dir[byte]&bit) != 0) {
		pageframe_dir[byte] &= ~bit;
	}
}

uint64_t pf2addr(uint64_t frame) {
	return frame*0x1000;
}

uint64_t addr2pf(uint64_t addr) {
	return addr/0x1000;
}

extern void load_pml4t(struct PML4T*);

struct PML4T* pml4t;
void setup_paging(struct PML4T *ptr) {
	kmsg(NORET INFO "Pos of PML4T: 0x");
	PrintInt((uint64_t)&ptr, 16);
	PrintChar('\n');
	pml4t = ptr;
	load_pml4t(ptr);
}

void first_n_pages(uint64_t n) {
	for (int i = 0; i < n%513; ++i)
	{
		kmsg(NORET INFO "Page "); PrintInt(i, 10); PrintStr(" = 0x");
		PrintInt((uint64_t)pml4t->pdpt[0]->pdt[i], 16);
		if (i != 0 && (i-1)%2 == 0)
			PrintChar('\n');
	}
	PrintChar('\n');
}