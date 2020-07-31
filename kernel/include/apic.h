#include <efihelper.h>
#include <io.h>
#include <stdbool.h>

// TODO: implement APIC
#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100 // Processor is a BSP
#define IA32_APIC_BASE_MSR_ENABLE 0x800

#define CPUID_FEAT_EDX_APIC 1<<9

bool setup_apic();

// Legacy PIC support if APIC is not supported
#define PIC1 0x20
#define PIC2 0xA0

#define PIC1_C 0x20
#define PIC2_C 0xA0

#define PIC1_D 0x21
#define PIC2_D 0xA1

void remap_pic(int offset1, int offset2);
void mask_pic();
void setup_pic();
void int_end(bool both);