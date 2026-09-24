#ifndef L_PMM_H
#define L_PMM_H

#include <multiboot2.h>
#include <stddef.h>
#include <stdint.h>

struct memoryblock {
	void* base_addr;
	size_t size;
	uint8_t type;
};

typedef struct {
	uint32_t length;
        struct memoryblock blocks[];
	#define MEMBANK_ESIZE (sizeof(void*) + sizeof(size_t) + sizeof(uint8_t))
	#define BankSize(len) (4 + (MEMBANK_ESIZE * len))
} MemoryBlockBank_t;
extern MemoryBlockBank_t* memBlockBank;

typedef struct ReservedBlock {
	void* addr;
	size_t size;
} ReservedBlockMarker;

extern ReservedBlockMarker rbm_Kernel;
extern ReservedBlockMarker rbm_MultibootInfo;

void initialize_memBlockBank(struct multiboot_tag_mmap* tag_mmap);

#endif
