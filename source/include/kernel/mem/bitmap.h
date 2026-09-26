#ifndef L_BITMAP_H
#define L_BITMAP_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint32_t *data;
    size_t size;
    uint8_t partial;
} bitmap_t;
#define BITMAP_size(x)		((x / 32) + ((x % 32) ? 1 : 0 ))
#define BITMAP_partial(x)	(x % 32) // Get amount of stray bits in last chunk

static inline void bitmap_initP(bitmap_t *bmp) {
        if (bmp->partial > 0 && bmp->size > 0) {
		uint32_t mask = ~0U << bmp->partial;
		bmp->data[bmp->size - 1] |= mask;
        }
}  

static inline void bitmap_set(bitmap_t *bmp, size_t bit_idx) {
    bmp->data[bit_idx / 32] |= (1U << (bit_idx % 32));
}

static inline uint8_t bitmap_test(const bitmap_t *bmp, size_t bit_idx) {
    return (bmp->data[bit_idx / 32] & (1U << (bit_idx % 32))) != 0;
}

uint8_t bitmap_find_first_free(const bitmap_t *bmp, size_t *out_idx);

#endif
