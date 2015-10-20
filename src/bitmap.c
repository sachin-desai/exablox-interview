#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitmap.h"

#define ROUND_UP(_x, _y)        ((((_x) + ((_y) - 1)) / (_y)) * (_y))
#define ROUND_DOWN(_x, _y)      (((_x) / (_y)) * (_y))

/*
 * bm_is_initialized
 * 	Check if bm is initialized
 *
 * input
 * 	bm: user supplied bm structure
 *
 * output:
 * 	1 if true
 * 	0 if false
 */
int bm_is_initialized(bitmap *bm)
{
	if (bm->bm_magic == BITMAP_MAGIC)
		return 1;
	else
		return 0;
}

/*
 * bm_create
 * 	Create a bitmask structure.
 *
 * input:
 * 	bm:       Pointer to the bm structure.
 *	bm_num_elems: Number of elements in the bm structure.
 *
 * output:
 *   0 if bm creation successful
 *  -1 if bm creation failed
 */
int bm_create(bitmap *bm, unsigned long bm_num_elems)
{
	unsigned int num_slots;

	if (bm == NULL)
		return -1;

	if (bm_num_elems <= 0)
		return -1;

	num_slots = ROUND_UP(bm_num_elems / sizeof(unsigned long), sizeof(unsigned long));
	bm->bm_array = malloc(num_slots);

	if (bm->bm_array == NULL)
		return -1;

	bm->bm_num_elems = bm_num_elems;
	bm->bm_array_size = ((bm_num_elems - 1) / BITS_PER_QWORD) + 1;

	memset(bm->bm_array, 0,
		bm->bm_array_size * sizeof(*bm->bm_array));
	bm->bm_magic = BITMAP_MAGIC;

	return 0;
}

/*
 * array_index
 * 	Given a bit, return the array index.
 *
 * input:
 * 	bit:     bit index.
 *
 * output value:
 *  -1 if bm creation failed.
 */
static inline unsigned long array_index(unsigned long bit)
{
	return (bit >> BIT_TO_INDEX_SHIFT);
}

/*
 * mask_for_bit
 * 	Given a bit, return the bitmask of size unsigned long.
 *
 * input:
 * 	bit:     bit index.
 *
 * output:
 * 	bitmask value.
 */
static unsigned long mask_for_bit(unsigned long bit)
{
	/* The & is equivalent to % BITS_PER_QWORD but avoids a divide. */
	return (1L << (bit & (BITS_PER_QWORD - 1)));
}

/*
 * bm_destroy
 * 	Cleanup an initialized bitmask
 *
 * input:
 * 	bitmask: pointer to the bitmask.
 *
 * output:
 *  	n/a
 */
void bm_destroy(bitmap *bm)
{
	if (bm_is_initialized(bm)) {

		free(bm->bm_array);
		bm->bm_array = NULL;
		bm->bm_magic = 0;
	}
}

/*
 * bm_set
 * 	Set a bit index represented by elem in the bm
 *
 * input:
 * 	bm: bm pointer 
 * 	elem:   bit index to set.
 *
 * output:
 *	n/a
 */
void bm_set(bitmap *bm, unsigned long elem)
{
	unsigned long mask = mask_for_bit(elem);
	unsigned long index = array_index(elem);

	/* set the bit in the bm. */
	bm->bm_array[index] |= mask;

}

/*
 * bm_clear
 * 	Clear a bit index represented by elem in the bm.
 *
 * input:
 * 	bm: bm pointer
 * 	elem:   bit index to clear
 *
 * output:
 *	n/a 
 */
void bm_clear(bitmap *bm, unsigned long elem)
{
	unsigned long mask = mask_for_bit(elem);
	unsigned long index = array_index(elem);

	/* set the bit in the bm. */
	bm->bm_array[index] &= ~mask;

}

/*
 * bm_clear_all
 * 	Clear all the entries in the bm.
 *
 * input:
 * 	bm: bm pointer
 *
 * output:
 * 	n/a
 */
void bm_clear_all(bitmap *bm)
{
	memset(bm->bm_array, 0,
		bm->bm_array_size * sizeof *bm->bm_array);
}

/*
 * bm_clear_range
 * 	Clear a range of bits in the bm
 *
 * input:
 * 	bm: bm pointer
 * 	start: start floppy to be cleared.
 * 	range: range of floppies that needs to be cleared.
 *
 * output:
 * 	n/a
 */
void bm_clear_range(bitmap *bm,
			unsigned long start, unsigned long range)
{
	int i;
	for (i = start; i < start + range; i++)
		bm_clear(bm, i);
}

/*
 * bm_is_set
 * 	Given a bit index, check if that is set in the bm.
 *
 * input:
 * 	bm: bm pointer
 * 	elem:   bit index to verify if set.
 *
 * output:
 *   0 if bit is not set.
 *   1 if bit is set .
 *  -1 if invalid bit.
 */
int bm_is_set(bitmap *bm, unsigned long elem)
{
	unsigned long mask = mask_for_bit(elem);
	unsigned long index = array_index(elem);

	if (elem > bm->bm_num_elems)
		return -1;

	if (bm->bm_array[index] & mask)
		return 1;
	else
		return 0;
}
