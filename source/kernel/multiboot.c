#include <kernel/multiboot.h>

/* Multiboot 2, unlike the original specification, uses tags
*  to organize its contents. Each tag has an associated type,
*  represented by a 32-bit integer. This function finds a tag
*  given its type. */
struct multiboot_tag *multiboot_find_tag(struct multiboot_info_block* mbi, uint32_t type) {
    /* The multiboot info structure begins with a 32-bit integer
    *  indicating the total size of the structure. This is then
    *  followed by a 32-bit reserved region and then by the tags. */
	struct multiboot_tag *tag = (void *)mbi + 8;
	while ((void *)tag < (void *)mbi + mbi->total_size) {
		if (tag->type == type) {
			return tag;
		}
		tag = (void *)tag + tag->size;
		/* Tags are always aligned on 8-byte boundaries. */
		if ((uintptr_t)tag % 8 > 0) {
			tag = (void *)tag + 8 - ((uintptr_t)tag % 8);
		}
	}
	return 0;
}
