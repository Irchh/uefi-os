#ifndef __MEM_H_
#define __MEM_H_

#include <stddef.h>
#include <efihelper.h>

struct pagedir_s {
	
};

typedef struct __mem_chk_info
{
	size_t size;
	size_t free;
	struct __mem_chk_info* prev;
	struct __mem_chk_info* next;
} mblki;

void init_mem_after_bs();
void* kmalloc (size_t size);
void kfree (void *pool);

#endif //__MEM_H_