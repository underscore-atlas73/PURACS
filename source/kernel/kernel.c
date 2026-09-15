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

void kmain(uint32_t magic, uint32_t* mbi) {
        terminal_init(&terminals[0], (void*)CGA_BUF, 80*25*2);	// Initialize temporary direct debug terminal

	////////////////MULTIBOOT///////////////////
        switch (magic) {
		case 0x2BADB002:
			puts("Loaded by Multiboot1 program. This version was built for the Multiboot2 Specification; the kernel will abort.");
                        return;
		case MULTIBOOT2_BOOTLOADER_MAGIC /*0x36D76289*/:
			puts("Loaded by Multiboot2 program.");
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

	while (1) {
	}

}
