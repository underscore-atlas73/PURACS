#ifndef L_PMM_H
#define L_PMM_H

#include <multiboot2.h>
#include <kernel/multiboot.h>
#include <stddef.h>
#include <stdint.h>
#include <kernel/mem/bitmap.h>
#include <stdio.h>

extern bitmap_t pmmMap; // Special opaque type for PMM object?

struct memoryblock {
	void* base_addr;
	size_t size;
	uint8_t type;
} __attribute__ ((__packed__)); // I can afford to waste alignment-efficiency since the main array (memBlockBank) is only an initial scratch variable

typedef struct {
	uint32_t length;
        struct memoryblock blocks[];
	#define MEMBANK_ESIZE sizeof(struct memoryblock)
	#define BankSize(len) (4 + (MEMBANK_ESIZE * len))
} MemoryBlockBank_t;
extern MemoryBlockBank_t* memBlockBank;

typedef struct ReservedBlock {
	void* addr;
	size_t size;
} ReservedBlockMarker;

#define RBM_Kernel		0
#define RBM_MultibootInfo	1
#define RBM_memBlockBank	2
#define RBM_PMMledger		3
extern ReservedBlockMarker RBM[16]; 

void initialize_PMM(struct multiboot_info_block* mbi, struct multiboot_tag_mmap* tag_mmap);

#endif
