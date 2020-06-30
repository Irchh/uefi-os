#ifndef __MEM_H_
#define __MEM_H_

#include <stdint.h>
#include <efihelper.h>

void init_mem_after_bs();
void* malloc (size_t size);
void free (void *pool);

#endif //__MEM_H_