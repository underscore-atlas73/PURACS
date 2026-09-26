#include "kernel/mem/bitmap.h"
#include <kernel/mem/pmm.h>
#include <stdint.h>

bitmap_t pmmMap = {0};
MemoryBlockBank_t* memBlockBank = NULL;

ReservedBlockMarker rbm_Kernel = {0};
ReservedBlockMarker rbm_MultibootInfo = {0};

void initialize_PMM(struct multiboot_tag_mmap* tag_mmap) {
	// first lsmmap entry
	struct multiboot_mmap_entry *entry =
		(struct multiboot_mmap_entry *)(uintptr_t)tag_mmap->entries;

	// find first AVAILABLE memory range
	while ((void *)entry < (void *)tag_mmap + tag_mmap->size &&
		entry->type != MULTIBOOT_MEMORY_AVAILABLE) entry = (void *)entry + tag_mmap->entry_size;

	// skip Real Mode & BDA / NULL Page
	if (entry->addr < 0x1000) memBlockBank = (void *)0x1000;
	else memBlockBank = (void *)entry->addr;

	// calculate amount of stored entries
	memBlockBank->length = (tag_mmap->size - sizeof(struct multiboot_tag_mmap)) / tag_mmap->entry_size;

        // copy lsmmap to memBlockBank
        uint32_t i = 0;
	while ((void *)entry < (void *)tag_mmap + tag_mmap->size) {
		memBlockBank->blocks[i].base_addr	= (void*)entry->addr;
		memBlockBank->blocks[i].type		= entry->type;
		memBlockBank->blocks[i].size		= entry->len;

		entry = (void *)entry + tag_mmap->entry_size;
		i++;
	}

        for (i = memBlockBank->length; i != 0; i--) {
		if (memBlockBank->blocks[i].type == 1) {
			pmmMap.size = BITMAP_size(((uintptr_t)memBlockBank->blocks[i].base_addr + (uintptr_t)memBlockBank->blocks[i].size) / 4096);
			break;
                }
        }
        pmmMap.size = (pmmMap.size > 16384) ? 16384 : pmmMap.size; // Hard cap at 8GiB for now
        pmmMap.data = (uint32_t *)((void *)memBlockBank + ((BankSize(memBlockBank->length) + 31) & ~31));
        bitmap_initP(&pmmMap);
}
