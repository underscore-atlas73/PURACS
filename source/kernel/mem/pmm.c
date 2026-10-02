#include <kernel/mem/pmm.h>

////////////////////////MEMORYBLOCKBANK/////////////////////////
struct memoryBlock {
	void* base_addr;
	size_t size;
	uint8_t type;
} __attribute__ ((__packed__)); // I can afford to waste alignment-efficiency since the main array (memBlockBank) is only an initial scratch variable

typedef struct {
	uint32_t length;
        struct memoryBlock blocks[];
	#define MEMBANK_ESIZE sizeof(struct memoryBlock)
	#define BankSize(len) (4 + (MEMBANK_ESIZE * len))
} MemoryBlockBank_t;
MemoryBlockBank_t* memBlockBank = NULL;

int8_t initialize_memBlockBank(struct multiboot_info_block* mbi) {
        struct multiboot_tag_mmap *tag_mmap =
            (struct multiboot_tag_mmap *)multiboot_find_tag(mbi, MULTIBOOT_TAG_TYPE_MMAP);
        if (!tag_mmap) {
		puts("MULTIBOOT_TAG_TYPE_MMAP NOT FOUND! STALL.");
		return -1;
        }

	// first lsmmap entry
	struct multiboot_mmap_entry *entry =
		(struct multiboot_mmap_entry *)(uintptr_t)tag_mmap->entries;

	// find first AVAILABLE memory range
	while ((void *)entry < (void *)tag_mmap + tag_mmap->size &&
		entry->type != MULTIBOOT_MEMORY_AVAILABLE) entry = (void *)entry + tag_mmap->entry_size;

	// skip Real Mode & BDA / NULL Page
	if (entry->addr < 0x1000) memBlockBank = (void *)0x1000;
	else memBlockBank = (void *)entry->addr;

	// copy lsmmap to memBlockBank
	size_t i = 0;
	while ((void *)entry < (void *)tag_mmap + tag_mmap->size) {
		memBlockBank->blocks[i].base_addr	= (void*)entry->addr;
		memBlockBank->blocks[i].type		= entry->type;
		memBlockBank->blocks[i].size		= entry->len;

		entry = (void *)entry + tag_mmap->entry_size;
		i++;
	}

	// calculate amount of stored entries
	memBlockBank->length = (tag_mmap->size - sizeof(struct multiboot_tag_mmap)) / tag_mmap->entry_size;
	
        printf("MemoryBlockBank (Addr: %p ; Length: %i entries):\n",
		memBlockBank, memBlockBank->length);
        for (size_t i = 0; i < memBlockBank->length; i++) {
		printf("\tAddr: %p ; Size: %u bytes ; Type: %i\n", memBlockBank->blocks[i].base_addr, memBlockBank->blocks[i].size, memBlockBank->blocks[i].type);
        }

	return 0;
}
////////////////////////////////////////////////////////////////
/////////////////////PHYSICALMEMORYMANAGER//////////////////////
uint8_t instCount;
typedef struct PMM {
	size_t total_memory;
	size_t free_pages;
	
	size_t rsrvBlockRegSize;
	ReservedBlockMarker_t rsrvBlockReg[16];	// Boot-time reserved blocks (formerly RBM)
	
	size_t mapsSize;
	bitmap_t maps[16];			
} PhysMemMgr_t;
						// Theoretically, more MAY be necessary with the x64 target's address space of 2^64 
						//	(needing 2^31 bitmaps of representing 8GiB...)
						// Better idea: large bitmaps post-boot

PhysMemMgr_t* initialize_PMM(struct multiboot_info_block* mbi) {
	PhysMemMgr_t* PMM = (void*)memBlockBank + BankSize(memBlockBank->length);
	
	PMM->rsrvBlockReg[RBR_MultibootInfo].addr = mbi;
        PMM->rsrvBlockReg[RBR_MultibootInfo].size = mbi->total_size;
        PMM->rsrvBlockReg[RBR_Kernel].addr = (void*)KERNEL_BIN_START;
        PMM->rsrvBlockReg[RBR_Kernel].size = KERNEL_BIN_END - KERNEL_BIN_START;
	PMM->rsrvBlockReg[RBR_memBlockBank].addr = memBlockBank;
	PMM->rsrvBlockReg[RBR_memBlockBank].size = BankSize(memBlockBank->length);

	// gauge max RAM using last available range
	size_t i;
	for (i = memBlockBank->length; i != 0; i--) {
		if (memBlockBank->blocks[i - 1].type == 1) {
			PMM->total_memory = (uintptr_t)memBlockBank->blocks[i - 1].base_addr + (uintptr_t)memBlockBank->blocks[i - 1].size;
			PMM->free_pages = PMM->total_memory / 4096;
			break;
		}
	}

	// Set up initial mapping
	PMM->mapsSize = 1;
	PMM->maps[0].size	= BITMAP_size((PMM->free_pages >= 0x100000) ? 0x100000 : PMM->free_pages, 2);
	PMM->maps[0].partial	= BITMAP_partial((PMM->free_pages >= 0x100000) ? 0x100000 : PMM->free_pages, 2);
	PMM->maps[0].bpe	= 2;

	PMM->maps[0].data = (uint32_t *)(((uintptr_t)&PMM + sizeof(struct PMM) + 31) & ~31);
	bitmap_initP(&PMM->maps[0]);

	PMM->rsrvBlockReg[RBR_INITledger].addr = PMM->maps[0].data;
	PMM->rsrvBlockReg[RBR_INITledger].size = PMM->maps[0].size * 4;
	PMM->rsrvBlockRegSize = 4;

	//printf("%X, %X", PMM->maps[0].size, PMM->maps[0].bpe);
	//for(;;);

	// initalize pmmMap
	size_t consErrs = 0;
	bitmap_write(&PMM->maps[0], 0, PMM_SINGLE); // Always mark zero page as unavailable
	for (i = 1; i < (((PMM->maps[0].size - 1) * 32) + PMM->maps[0].partial) / PMM->maps[0].bpe; i++) { // Check exclusions first; crawl memBlockBank
		size_t R = 0;
		for (; R < PMM->rsrvBlockRegSize; R++) {
			if ((uintptr_t)PMM->rsrvBlockReg[R].addr + PMM->rsrvBlockReg[R].size <= i*0x1000+0xFFF) // check if upper bound of restriction is in page
				if ((uintptr_t)PMM->rsrvBlockReg[R].addr + PMM->rsrvBlockReg[R].size > i*0x1000) {
					bitmap_write(&PMM->maps[0], i, PMM_SINGLE); 
					goto CONTINUE;
				}
			if ((uintptr_t)PMM->rsrvBlockReg[R].addr <= i*0x1000+0xFFF) // check if lower bound of restriction is in page
				if ((uintptr_t)PMM->rsrvBlockReg[R].addr >= i*0x1000) {
					bitmap_write(&PMM->maps[0], i, PMM_SINGLE); 
					goto CONTINUE;
				}

			if (i*0x1000 < (uintptr_t)PMM->rsrvBlockReg[R].addr + PMM->rsrvBlockReg[R].size) // check if page is CONTAINED by exclusion
				if (i*0x1000 >= (uintptr_t)PMM->rsrvBlockReg[R].addr) {
					bitmap_write(&PMM->maps[0], i, PMM_SINGLE);
					goto CONTINUE;
				}
		}

		struct memoryBlock* j = &(memBlockBank->blocks[memBlockBank->length / 2]);
		uint8_t lastD = 0;
		for (; (void*)j > (void*)memBlockBank && (void*)j < (void*)memBlockBank + BankSize(memBlockBank->length);) {
			uint8_t ubound = (uintptr_t)j->base_addr + j->size <= i*0x1000+0xFFF;	// check upper bound of range first
			ubound |= ((uintptr_t)j->base_addr + j->size > i*0x1000) << 1;		// :
			uint8_t bound = ((uintptr_t)j->base_addr <= i*0x1000+0xFFF);		// check lower bound of range
			bound |= ((uintptr_t)j->base_addr >= i*0x1000) << 1;			// :

			if ((bound == 0b11 || ubound == 0b11) || (bound == 0b01 && ubound == 0b10)) { // check pages that contain boundries AND are contained in boundries 
				if (j->type != 1) bitmap_write(&PMM->maps[0], i, PMM_SINGLE); 
				else bitmap_write(&PMM->maps[0], i, PMM_AVAILABLE);
				if (consErrs >= 1024) printf("Ignored %u dead pages.\n", consErrs);
				consErrs = 0;
				goto CONTINUE;
			}
			if (bound == 0b10) { // base addr is above page
				j -= 1;
				if (lastD == 0b01) goto OSCILLATE;
				lastD = 0b10;
			}
				
			if (bound == 0b01) { // base addr is below page
				j += 1;
				if (lastD == 0b10) goto OSCILLATE;
				lastD = 0b01;
			}
		}

		if (consErrs < 1024) { 
			printf("Error validating page %u (memory address %X).\n", i, i * 0x1000);
			if (consErrs == 1024) puts("Dead sea detected. Ignoring dOut until next valid page.");
		}
		consErrs++;
		bitmap_write(&PMM->maps[0], i, PMM_SINGLE);
		goto CONTINUE;

		OSCILLATE:
		if (consErrs < 1024) {
			printf("Oscillation detected during crawl for page %u (memory address %X). \n\t(Most likely unmapped address.)\n", i, i * 0x1000);
			if (consErrs == 1024) puts("Dead sea detected. Ignoring dOut until next valid page.");
		}
		consErrs++;
		bitmap_write(&PMM->maps[0], i, PMM_SINGLE);

		CONTINUE: ;
	}

        printf("Reserved Sections:\n");
        printf("\tAddr: %p ; Size: %u bytes\n", PMM->rsrvBlockReg[RBR_Kernel].addr, PMM->rsrvBlockReg[RBR_Kernel].size);
        printf("\tAddr: %p ; Size: %u bytes\n", PMM->rsrvBlockReg[RBR_MultibootInfo].addr, PMM->rsrvBlockReg[RBR_MultibootInfo].size);
        printf("\nInitial Ledger Memory: %p bytes\n", (size_t)PMM->maps[0].size * 4 * (8 / PMM->maps[0].bpe) * 4096);
        printf("\t(Init Ledger Start: %X)\n", PMM->maps[0].data);

	return PMM;
}
////////////////////////////////////////////////////////////////
