/*OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386:x86-64)*/

ENTRY(_start)

SECTIONS
{
	. = 1M;
	__KERNEL_BIN_START = .;

	.multiboot_header BLOCK(4K) : {
		*(.multiboot_header)
	}
	
	.text BLOCK(4K) : {
		*(.text)
	}

	.rodata BLOCK(4K) : {
		*(.rodata)
        }

	.data BLOCK(4K) : {
		*(.data)
	}

	.bss BLOCK(4K) : {
		*(COMMON)
		*(.bss)
	}

	__KERNEL_BIN_END = .;
}
