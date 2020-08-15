#include <stdint.h>
#include <efi.h>

extern UINTN mapSize, descriptorSize;
extern EFI_MEMORY_DESCRIPTOR *memoryMap;
extern UINT32 descriptorVersion;

EFI_STATUS exit_services();