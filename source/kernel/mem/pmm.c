#include <kernel/mem/pmm.h>

bitmap_t pmmMap = {0};
MemoryBlockBank_t* memBlockBank = NULL;

ReservedBlockMarker RBM[16] = {0};

void initialize_PMM(struct multiboot_info_block* mbi, struct multiboot_tag_mmap* tag_mmap) {
        RBM[RBM_MultibootInfo].addr = mbi;
        RBM[RBM_MultibootInfo].size = mbi->total_size;
        RBM[RBM_Kernel].addr = (void*)KERNEL_BIN_START;
        RBM[RBM_Kernel].size = KERNEL_BIN_END - KERNEL_BIN_START;

	// first lsmmap entry
	struct multiboot_mmap_entry *entry =
		(struct multiboot_mmap_entry *)(uintptr_t)tag_mmap->entries;

	// find first AVAILABLE memory range
	while ((void *)entry < (void *)tag_mmap + tag_mmap->size &&
		entry->type != MULTIBOOT_MEMORY_AVAILABLE) entry = (void *)entry + tag_mmap->entry_size;

	// skip Real Mode & BDA / NULL Page
	if (entry->addr < 0x1000) memBlockBank = (void *)0x1000;
	else memBlockBank = (void *)entry->addr;
	RBM[RBM_memBlockBank].addr = memBlockBank; // set rbm base address for mBB

	// calculate amount of stored entries
	memBlockBank->length = (tag_mmap->size - sizeof(struct multiboot_tag_mmap)) / tag_mmap->entry_size;
	RBM[RBM_memBlockBank].size = BankSize(memBlockBank->length); // set size of rbm for mBB

	// copy lsmmap to memBlockBank
	size_t i = 0;
	while ((void *)entry < (void *)tag_mmap + tag_mmap->size) {
		memBlockBank->blocks[i].base_addr	= (void*)entry->addr;
		memBlockBank->blocks[i].type		= entry->type;
		memBlockBank->blocks[i].size		= entry->len;

		entry = (void *)entry + tag_mmap->entry_size;
		i++;
	}

	// gauge max RAM using last available range
	for (i = memBlockBank->length; i != 0; i--) {
		if (memBlockBank->blocks[i].type == 1) {
			pmmMap.size = BITMAP_size(((uintptr_t)memBlockBank->blocks[i].base_addr + (uintptr_t)memBlockBank->blocks[i].size) / 4096);
			break;
		}
	}
	// hard cap at 8GiB (we have around 638KiB in lowmem guaranteed to use for the bitmap; 8GiB uses a safe 256KiB)
	if (pmmMap.size > 16384) {
		pmmMap.size = (pmmMap.size > 16384) ? 16384 : pmmMap.size;
		pmmMap.partial = 0;
	}
	// set bmp data pointer to first free location (4 byte aligned for speed)
	pmmMap.data = (uint32_t *)(((uintptr_t)memBlockBank + BankSize(memBlockBank->length) + 31) & ~31);
	bitmap_initP(&pmmMap);

	RBM[RBM_PMMledger].addr = pmmMap.data;
	RBM[RBM_PMMledger].size = pmmMap.size;

	// initalize pmmMap
	bitmap_set(&pmmMap, 1); // Always mark zero page as unavailable
	for (i = 1; i < pmmMap.size * 32; i++) { // Check exclusions first; crawl  memBlockBank
		
		size_t R = 0;
		for (; R < 16; R++) {
			if ((uintptr_t)RBM[R].addr + RBM[R].size <= i*0x1000+0xFFF) // check if upper bound of restriction is in page
				if ((uintptr_t)RBM[R].addr + RBM[R].size >= i*0x1000) {bitmap_set(&pmmMap, i); goto CONTINUE;}
			if ((uintptr_t)RBM[R].addr <= i*0x1000+0xFFF) // check if lower bound of restriction is in page
				if ((uintptr_t)RBM[R].addr >= i*0x1000) {bitmap_set(&pmmMap, i); goto CONTINUE;}
		}

		struct memoryblock* j = &(memBlockBank->blocks[memBlockBank->length / 2]);
		uint8_t lastD = 0;
		for (; (void*)j > (void*)memBlockBank && (void*)j < (void*)memBlockBank + BankSize(memBlockBank->length);) {
			uint8_t bounds = ((void*)(i*0x1000) < (void*)j->base_addr + j->size); // check upper bound first
			bounds |= ((void*)(i*0x1000) >= (void*)j->base_addr) << 1; // pack lower bound check into upper bit
			
			if (bounds == 0b11) { 
				if (j->type == 2) bitmap_set(&pmmMap, i); 
				else bitmap_clear(&pmmMap, i); 
				goto CONTINUE;
			}
			if (bounds == 0b10) { 
				j += 1;
				if (lastD == 0b01) goto OSCILLATE;
				lastD = 0b10;
			}
				
			if (bounds == 0b01) {
				j -= 1;
				if (lastD == 0b10) goto OSCILLATE;
				lastD = 0b01;
			}
		}

		printf("Error validating bit %u (memory address %X).\n", i, i * 0x1000);
		bitmap_set(&pmmMap, i);
		goto CONTINUE;

		OSCILLATE:
		printf("Oscillation detected during crawl for bit %u (memory address %X). \n\t(Most likely unmapped address.)\n", i, i * 0x1000);
		bitmap_set(&pmmMap, i);
		CONTINUE: ;
	}
}
