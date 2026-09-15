.section .multiboot_header, "a"
header_start:
	.int 0xE85250D6
	.int 0
	.int header_end - header_start
	.int 0x100000000 - (0xE85250D6 + 0 + (header_end - header_start))

	#END
	.word 0
	.word 0
	.int 8
header_end:

.section .text
.code32
.global _start
.type _start, @function
#.include "include/CPUID.asm"
_start:
	cli 			# dis interrupts
	mov $stack_top, %esp	# init stack pointer

	# psuh multiboot2 pointer and magic
	push %ebx
	push %eax

	# zero out tables (4-level: PML4 -> PDPT -> PD -> PT (-> 4K Page))
	movl $PML4_TABLE, %edi
	movl $3072, %ecx #stosl writes 4 byte values, 4 x 3072 = 12288, 3 x 4096 = 12288
	xorl %eax, %eax
	rep stosl

	# identity map first few mb: direct PML4E_0 to initial PDPT
	movl $PAGE_DIRECTORY_POINTER_TABLE, %eax
	orl $0b11, %eax
	movl %eax, (PML4_TABLE)

	# point PDPT to Page Directory
	movl $PAGE_DIRECTORY, %eax
	orl $0b11, %eax
	movl %eax, (PAGE_DIRECTORY_POINTER_TABLE)

	# Identity map first 2MiB page
	movl $0x83, %eax			# Magic value 0x83 = 10000011: P.S. = 1 ; R/W = 1 ; Present = 1
	movl %eax, PAGE_DIRECTORY(,%ecx, 8)	# displacement(base, index, scale)
	movl $0, PAGE_DIRECTORY + 4(,%ecx,8)

	# Enable PAE
	movl %cr4, %eax
	orl $1 << 5, %eax	# CR4.PAE is bit 5
	movl %eax, %cr4

	# Load PML4 address into CR3
	movl $PML4_TABLE, %eax
	movl %eax, %cr3

	# Enable long mode
	movl $0xC0000080, %ecx
	rdmsr
	orl $1 << 8, %eax	# enable EFER.LME, bit 8
	wrmsr

	# Finally enable paging
	movl %cr0, %eax
	orl $1 << 31, %eax	# CR0.PG, bit 31; enables paging
	movl %eax, %cr0

	lgdt GDT64_POINTER

	popl %eax
	popl %ebx

	ljmp $0x08, $_start64
#END 32 BIT CODE
#64 bit yay
.code64
_start64:
	movl %eax, %edi		# x64 System V ABI registers; passes multiboot shtuff to kmain
	movl %ebx, %esi

	mov $0x0, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	#movq $0xB8000, %rax	# cga memory (thank GOD for identity mapping)
	#movb $'X', (%rax)	# display x
	#movb $0x0F, 1(%rax)	# one plus rax addr

	.extern kmain
	call kmain

	#should be unreachable, halt
1:	hlt
	jmp 1b

# gdt
.section .rodata
.align 8
GDT64:
	.quad 0x0000000000000000 # required null desc
	.quad 0x00209A0000000000 # code seg desc
	.quad 0x0000920000000000 # data seg desc
GDT64_POINTER:
	.word . - GDT64 - 1
	.quad GDT64
.global __KERNEL_GDT
__KERNEL_GDT:
	.byte 0x08

# stack and page tables
.section .bss
.align 4096
.global PML4_TABLE
.global PAGE_DIRECTORY_POINTER_TABLE
.global PAGE_TABLE
PML4_TABLE: .skip 4096
PAGE_DIRECTORY_POINTER_TABLE: .skip 4096
PAGE_DIRECTORY: .skip 4096

stack_bottom:
	.skip 8192
stack_top:
