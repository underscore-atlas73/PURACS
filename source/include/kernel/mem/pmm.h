#ifndef L_PMM_H
#define L_PMM_H

#include <multiboot2.h>
#include <kernel/multiboot.h>
#include <stddef.h>
#include <stdint.h>
#include <kernel/mem/bitmap.h>
#include <stdio.h>

int8_t initialize_memBlockBank(struct multiboot_info_block* mbi);

typedef struct {
	void* addr;
	size_t size;
} ReservedBlockMarker_t;

#define RBR_Kernel		0
#define RBR_MultibootInfo	1
#define RBR_memBlockBank	2
#define RBR_INITledger		3

#define PMM_MAXINST		1	// Redundant but there might be some cool uses idk

#define PMM_AVAILABLE		0b00
#define PMM_HEAD		0b01
#define PMM_TRAIL		0b10
#define PMM_SINGLE		0b11
typedef struct PMM PhysMemMgr_t;
PhysMemMgr_t* initialize_PMM(struct multiboot_info_block* mbi);

#endif
