#ifndef L_BITMAP_H
#define L_BITMAP_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
	uint32_t *data;
	size_t size;
	uint8_t partial;
	uint8_t bpe;		// Bits per entry (or data "bit)
} bitmap_t;
#define BITMAP_size(entries, bpe)		((((entries) * (bpe)) + 31) / 32)
#define BITMAP_partial(entries, bpe)		(((entries) * (bpe)) % 32) // Get amount of stray bits in last chunk

static inline void bitmap_initP(bitmap_t *bmp) {
        if (bmp->partial > 0 && bmp->size > 0) {
		uint32_t mask = ~0U << bmp->partial;
		bmp->data[bmp->size - 1] |= mask;
        }
}  

static inline int8_t bitmap_write(bitmap_t *bmp, size_t idx, uint32_t val) {
	size_t bit_idx = idx * bmp->bpe;
	size_t chunk = bit_idx / 32;
	size_t offset = bit_idx % 32;
	
	if (chunk >= bmp->size) return -1;
	if (chunk == bmp->size - 1 && bmp->partial > 0) if ((offset + bmp->bpe) > bmp->partial) return -1;

	uint32_t mask = (bmp->bpe == 32) ? 0xFFFFFFFF : (1U << bmp->bpe) - 1;

	bmp->data[chunk] &= ~(mask << offset);
	bmp->data[chunk] |= ((val & mask) << offset);
	return 0; 
}

static inline int8_t bitmap_read(bitmap_t *bmp, size_t idx) {
	size_t bit_idx = idx * bmp->bpe;
	size_t chunk = bit_idx / 32;
	size_t offset = bit_idx % 32;

	if (chunk >= bmp->size) return 0;

	uint32_t mask = (bmp->bpe == 32) ? 0xFFFFFFFF : (1U << bmp->bpe) - 1;
	return (bmp->data[chunk] >> offset) & mask;
}

int8_t bitmap_find_first_nfree(const bitmap_t *bmp, size_t n, size_t *out_idx);
static inline int8_t bitmap_find_first_free(const bitmap_t *bmp, size_t *out_idx) {
	return bitmap_find_first_nfree(bmp, 1, out_idx);
}

#endif
