#include <kernel/mem/bitmap.h>

uint8_t bitmap_find_first_free(const bitmap_t *bmp, size_t *out_idx) {
	size_t array_size = bmp->size;

	// Scan "chunk" by "chunk" first (much faster than scanning bit by bit)
	for (size_t i = 0; i < array_size; i++) {
		// 0xFFFFFFFF is all bits set
		// EDGE CASE: If in the partial, this optimization will never work. If the bitmap occupies a location where garbage data was previously located, this may even falsely skip the partial (or other chunks uninitialized).
		if (bmp->data[i] != 0xFFFFFFFF) {
			// find guaranteed zero bit
			for (size_t j = 0; j < 32; j++) {
				size_t bit_idx = (i * 32) + j;
		
				// Prevent out-of-bounds if bitmap does not occupy entire array
				if (j >= bmp->partial && i + 1 == array_size) {
					return 0; 
				}

				if ((bmp->data[i] & (1 << j)) == 0) {
					*out_idx = bit_idx;
					return 1;
				}
			}
		}
	}
	
	return 0; // No free bits found
}
