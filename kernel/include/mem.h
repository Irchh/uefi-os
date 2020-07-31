#ifndef __MEM_H_
#define __MEM_H_

#include <stddef.h>
#include <efihelper.h>

struct PT
{
	void* addr[512];
} __attribute__((aligned(4096)));

struct PDT
{
	struct PT* pt[512];
} __attribute__((aligned(4096)));

struct PDPT
{
	struct PDT* pdt[512];
} __attribute__((aligned(4096)));

struct PML4T
{
	struct PDPT* pdpt[512];
} __attribute__((aligned(4096)));

struct pagedir_s {
	
};

typedef struct __mem_chk_info
{
	size_t size;
	size_t free;
	struct __mem_chk_info* prev;
	struct __mem_chk_info* next;
} mblki;

void init_mem_after_bs(uint64_t pages);
void* kmalloc (size_t size);
void kfree (void *pool);
uint64_t first_free_frame();
uint64_t pageframe_alloc(uint64_t frame);
void pageframe_dealloc(uint64_t frame);
uint64_t pf2addr(uint64_t frame);
uint64_t addr2pf(uint64_t frame);
uint64_t read_cr3();
int setup_pfa(uint64_t pages);
void setup_paging(struct PML4T *pml4t);
void first_n_pages(uint64_t n);

#endif //__MEM_H_