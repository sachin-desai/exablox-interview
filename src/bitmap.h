#ifndef __BITMAP_H__
#define __BITMAP_H__

#define BITMAP_MAGIC 0xBEEFDEAD
#define BITS_PER_QWORD 64
#define BIT_TO_INDEX_SHIFT 6
#define INVALID_HINT -1

typedef struct bitmap {
    unsigned int 	bm_magic;
    unsigned long 	bm_num_elems;
    unsigned long	bm_array_size;
    unsigned long	*bm_array;
} bitmap;

int	bm_create(bitmap* bm, unsigned long num_elems);
void    bm_destroy(bitmap* bm);
void    bm_set(bitmap* bm, unsigned long elem);
void	bm_clear(bitmap* bm, unsigned long elem);
void	bm_clear_all(bitmap* bm);
void 	bm_clear_range(bitmap* bm,
			unsigned long start, unsigned long range);
int 	bm_is_set(bitmap* bm, unsigned long elem);
int 	bm_is_initialized(bitmap* bm);

#endif
