#include <kernel/mem/pmm.h>
#include <stdint.h>

struct MemoryBlockBank* memBlockBank;

void initialize_memBlockBank(struct multiboot_tag_mmap* tag_mmap) {
	struct multiboot_mmap_entry *entry =
		(struct multiboot_mmap_entry *)(uintptr_t)tag_mmap->entries;

	while ((void *)entry < (void *)tag_mmap + tag_mmap->size &&
		entry->type != MULTIBOOT_MEMORY_AVAILABLE) entry = (void *)entry + tag_mmap->entry_size;

	if (entry->addr < 0x1000) memBlockBank = (void *)0x1000;
	else memBlockBank = (void *)entry->addr;
	
	memBlockBank->length = tag_mmap->size / tag_mmap->entry_size;

	uint32_t i = 0;
	while ((void *)entry < (void *)tag_mmap + tag_mmap->size) {
		memBlockBank->blocks[i].base_addr	= (void*)entry->addr;
		memBlockBank->blocks[i].type		= entry->type;
		memBlockBank->blocks[i].size		= entry->len;

		entry = (void *)entry + tag_mmap->entry_size;
		i++;
	}
}
