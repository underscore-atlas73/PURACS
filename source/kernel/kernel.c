////////	PLATFORM INCLUDES	////////
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

	initialize_memBlockBank(mbi);
        PhysMemMgr_t* PMM = initialize_PMM(mbi);
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
		ksleep(UINT64_MAX - sysclock);
                printf("UNREACHABLE.");
	}

}
