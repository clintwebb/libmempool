/*
 
 	Mempool.   Used to keep a pool of allocated memory chunks,  This would be
 	used in projects where peices of memory are needed rapidly and often.
 	Usually for short-lived operations.  Rather than allocating and deallocatng
 	memory a lot, we allocate once, and then re-use those allocated chunks.

 	This is not optimal for all situations however, so care in design should be taken.

*/


#include "mempool.h"

#include <assert.h>
#include <stdlib.h>


//-----------------------------------------------------------------------------
// Initialise the buffer list.  It can be assumed that the list contains
// garbage.
void mempool_init(mempool_t *pool)
{
	assert(pool != NULL);
	
	pool->ready = NULL;
	pool->used  = NULL;
}


//-----------------------------------------------------------------------------
// Free the resources that are referenced by the list.   At this point, all
// objects in the pool should be in the 'ready' list, because they should have
// all been returned.
void mempool_free(mempool_t *pool)
{
	mempool_entry_t *tmp;
	assert(pool != NULL);

	// all objects should have been returned to the pool at this point.
	assert(pool->active == NULL);

	// now we go thru the 'ready' list and remove all the objects in it (which
	// technically, at this point, there should not be any empty slots in the
	// list.)
	while (pool->ready != NULL) {
		tmp = pool->ready;
		pool->ready = tmp->next;
		assert(tmp->data);
		assert(tmp->size > 0);
		free(tmp->data);
		free(tmp);
	}

	assert(pool->active == NULL);
	assert(pool->ready  == NULL);
}



//-----------------------------------------------------------------------------
// look at our list of objects and return the one that is the best fit.  If it 
// finds one that is the exact size needed, it will use that.  Otherwise it 
// will use the one with the smallest allocation that will fit.
void * mempool_get(mempool_t *pool, unsigned int amount)
{
	mempool_entry_t *buff, *tmp, *best;
	
	assert(pool != NULL);
	assert(amount >= 0);

	buff = NULL;
	best = NULL;
	tmp = pool->ready;
	while (tmp && buff == NULL) {
		
		if (tmp->size == amount) {
			buff = tmp;
		}
		else {

			if (tmp->size > amount) {
				if ( best == NULL) {
					best = tmp;
				}
				else if (tmp->size < best->size) {
					best = tmp;
				}
			}

			tmp = tmp->next;
		}
	}

	if (buff == NULL && best != NULL) {
		buff = best;
	}

	// If we have a buff that we want to return, we need to remove it from the 
	// 'ready' list and put it on the 'used' list.
	if (buff) {
		if (pool->ready == buff) pool->ready = buff->next;
		if (buff->prev) buff->prev->next = buff->next;
		if (buff->next) buff->next->prev = buff->prev;
		buff->prev = NULL;
		buff->next = pool->used;
		pool->used = buff;

		assert(buff->data != NULL);
		assert(buff->size > 0);
		assert(buff->size >= amount;
		return(buff->data);
	}
	else {
		return(NULL);
	}
}


//-----------------------------------------------------------------------------
// Returns a previously provided buffer to the pool.  The buffer must have been
// allocated through the pool.   If the buffer being returned is larger than
// the pre-specified max for the pool, then it will be shrunk.  If max is zero,
// and the buffer is unusually large, then you should shrink it yourself before
// returning it.
//
//	** Should we free the buffer if it is larger than the max, or merely shrink
//	   it?  I fear that we would likely end up fragmenting memory much quicker
//	   if we only shrink it.
// 
void mempool_return(mempool_t *list, expbuf_t *buffer)
{
	int i;
	int found;
	
	assert(list != NULL);
	assert(buffer != NULL);

	assert(list->used.entries > 0 && list->used.list != NULL);
	assert((list->ready.entries == 0 && list->ready.list == NULL) || (list->ready.entries > 0 && list->ready.list != NULL));

	// check that the buffer is empty.
	assert(buffer->length == 0);

	assert(list->max >= 0);
	if (list->max > 0 && buffer->max > list->max) {
		expbuf_shrink(buffer, list->max);
	}

	// check that the returned buffer is in the 'used' list.
	// remove entry from 'used' list.
	found = -1;
	for (i=0; i<list->used.entries && found < 0; i++) {
		if (list->used.list[i] == buffer) {
			found = i;
			list->used.list[i] = NULL;
		}
	}
	assert(found >= 0);

	if (found >= 0) {
		// check that the returned buffer is not already in the 'ready' list.
		// add buffer to the 'ready' list.
		found = -1;
		for (i=0; i<list->ready.entries; i++) {
			assert(list->ready.list[i] != buffer);
			if (found < 0) {
				if (list->ready.list[i] == NULL) {
					found = i;
					list->ready.list[i] = buffer;
				}
			}
		}

		if (found < 0) {
			// didn't find an empty slot in the 'ready' list, so we need to make one.
			list->ready.list = (expbuf_t **) realloc(list->ready.list, sizeof(expbuf_t *) * (list->ready.entries + 1));
			list->ready.list[list->ready.entries] = buffer;
			list->ready.entries ++;
		}
	}
}






