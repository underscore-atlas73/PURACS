////////	PLATFORM INCLUDES	////////
#include "kernel/mem/bitmap.h"
#include <multiboot2.h>
#include <kernel/multiboot.h>
#include <kernel/INT/IDT.h>
#include <kernel/drivers/l_pic.h>
#include <kernel/drivers/pit.h>
////		MEMORY			    ////
#include <kernel/mem/pmm.h> 
////		INPUT			    ////
#include <kernel/drivers/l_kb.h>
////		OUTPUT			    ////
#include <kernel/drivers/l_vga.h>
#include <kernel/l_tty.h>
////////	PLibC			////////
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <PURACS/chrono.h>

void kmain(uint32_t magic, struct multiboot_info_block* mbi) {
        terminal_init(&terminals[0], (void*)CGA_BUF, 80*25*2);	// Initialize temporary direct debug terminal
  
	////////////////MULTIBOOT///////////////////
        switch (magic) {
		case 0x2BADB002:
			puts("Loaded by Multiboot1 program. This version was built for the Multiboot2 Specification; the kernel will abort.");
                        return;
		case MULTIBOOT2_BOOTLOADER_MAGIC /*0x36D76289*/:
			puts("Loaded by Multiboot2 program.\n");
                        break;
		default:
			printf("INVALID MAGIC: %X", magic);
			return;
        }

        struct multiboot_tag_mmap *tag_mmap =
            (struct multiboot_tag_mmap *)multiboot_find_tag(mbi, MULTIBOOT_TAG_TYPE_MMAP);
        if (!tag_mmap) {
		puts("MULTIBOOT_TAG_TYPE_MMAP NOT FOUND! STALL.");
		return;
        }

        initialize_PMM(tag_mmap);
        rbm_MultibootInfo.addr = mbi;
        rbm_MultibootInfo.size = mbi->total_size;
        rbm_Kernel.addr = (void*)KERNEL_BIN_START;
        rbm_Kernel.size = KERNEL_BIN_END - KERNEL_BIN_START;

        printf("MemoryBlockBank (Addr: %p ; Length: %i entries):\n",
		memBlockBank, memBlockBank->length);
        for (size_t i = 0; i < memBlockBank->length; i++) {
		printf("\tAddr: %p ; Size: %u bytes ; Type: %i\n", memBlockBank->blocks[i].base_addr, memBlockBank->blocks[i].size, memBlockBank->blocks[i].type);
        }
        
        printf("Reserved Sections:\n");
        printf("\tAddr: %p ; Size: %u bytes\n", rbm_Kernel.addr, rbm_Kernel.size);
        printf("\tAddr: %p ; Size: %u bytes\n", rbm_MultibootInfo.addr, rbm_MultibootInfo.size);
        printf("\nDetected Usable Memory: %X bytes\n", pmmMap.size * 4 * 8 * 4096);
        printf("Projected PMM Ledger Size: %X bytes\n", pmmMap.size * 4);
        printf("\t(Ledger Start: %X)\n", pmmMap.data);
        ////////////////////////////////////////////
	////////////////INTERRUPTS//////////////////
	idt_init();

	PIC_remap(0x20, 0x28);
	for (uint8_t i = 0; i < 16; i++) {
		PIC_set_mask(i);
	}

	pit_init(1000);
	PIC_clear_mask(PIC_PIT);
	PIC_clear_mask(PIC_KB);
	sti();
        ////////////////////////////////////////////
        ////////////////MEMORY//////////////////////
         
        ////////////////////////////////////////////
        while (1) {
		ksleep(INT64_MAX - sysclock);
                printf("UNREACHABLE.");
	}

}
