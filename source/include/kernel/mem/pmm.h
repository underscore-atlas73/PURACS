#ifndef L_PMM_H
#define L_PMM_H

#include <multiboot2.h>
#include <stddef.h>
#include <stdint.h>

struct MemoryBlockBank {
	uint32_t length;
        struct {
		void *base_addr;
                size_t size;
                uint8_t type;
        } blocks[];
};
extern struct MemoryBlockBank* memBlockBank;
 
void initialize_memBlockBank(struct multiboot_tag_mmap* tag_mmap);

#endif
