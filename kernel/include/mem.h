#ifndef __MEM_H_
#define __MEM_H_

#include <stdint.h>
#include <efi.h>

extern EFI_BOOT_SERVICES *gBS;

void* malloc (size_t size);
void free (void *pool);

#endif //__MEM_H_