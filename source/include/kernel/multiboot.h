#ifndef L_MULTIBOOT_H
#define L_MULTIBOOT_H

#include <multiboot2.h>
#include <stdint.h>

// Custom wrapper structure
struct multiboot_info_block
{
  uint32_t total_size;
  uint32_t reserved;
  //struct multiboot_tag tags[]; // tag sequence
};

struct multiboot_tag *multiboot_find_tag(struct multiboot_info_block* mbi, uint32_t type);

extern const void __KERNEL_BIN_START;
extern const void __KERNEL_BIN_END;
static const uintptr_t KERNEL_BIN_START	= (uintptr_t)&__KERNEL_BIN_START;
static const uintptr_t KERNEL_BIN_END = (uintptr_t)&__KERNEL_BIN_END;

#endif
