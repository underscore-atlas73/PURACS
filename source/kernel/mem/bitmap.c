#include <kernel/mem/bitmap.h>

int8_t bitmap_find_first_nfree(const bitmap_t *bmp, size_t n, size_t *out_idx) {
	if (n == 0) {
		return 0;
	}
	size_t array_size = bmp->size;
	size_t cons_free = 0;
	size_t start_idx = 0;
	uint32_t mask = (bmp->bpe == 32) ? -1U : (1U << bmp->bpe) - 1; 

	// Scan "chunk" by "chunk" first (much faster than scanning bit by bit)
        for (size_t i = 0; i < array_size; i++) {
		// 0xFFFFFFFF is all bits set; seek until potential find
		// EDGE CASE: If in the partial, this optimization will never work.
		//	If the bitmap occupies a location where garbage data was previously located, this may even falsely skip the partial (or other chunks uninitialized).
		if (cons_free == 0 && bmp->data[i] == 0xFFFFFFFF) { // cons_free should be 0; otherwise let the rest of the iteration handle it
			continue;
		}

		for (size_t j = 0; j < 32; j+=bmp->bpe) {
			uint32_t val = (bmp->data[i] >> j) & mask;

			if (val == 0) {
				if (cons_free == 0) {
					start_idx = ((i * 32) + j) / bmp->bpe; // Set 1st mark
				}

				cons_free++;
				if (cons_free == n) {
					*out_idx = start_idx;
					return 1;
				}
			} else {
				cons_free = 0;
			}
		}
	}
	
	return 0; // No free bits found
}

