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
	assert(pool->used == NULL);

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

	assert(pool->used == NULL);
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
	while (tmp) {
		
		if (tmp->size == amount) {
			buff = tmp;
			tmp = NULL;
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
		assert(buff->size >= amount);
		return(buff->data);
	}
	else {
		return(NULL);
	}
}


//-----------------------------------------------------------------------------
// Returns a previously provided pointer to the pool.  The pointer must have
// been retrieved through the pool.
void mempool_return(mempool_t *pool, void *ptr)
{
	mempool_entry_t *tmp, *entry;

	assert(pool);
	assert(ptr);
	assert(pool->used);

	// first try and find the pointer in the 'used' list.  Remove the object from it.
	entry = NULL;
	tmp = pool->used;
	while (tmp) {
		if (tmp->data == ptr) {
			entry = tmp;
			if (entry == pool->used) pool->used = entry->next;
			if (entry->prev) entry->prev->next = entry->next;
			if (entry->next) entry->next->prev = entry->prev;
			tmp = NULL;
		}

		tmp = tmp->next;
	}
	assert(entry != NULL);

	// then add the entry to the
	entry->prev = NULL;
	entry->next = pool->ready;
	pool->ready = entry;
}


//-----------------------------------------------------------------------------
// The memory pool system does not allocate new memory for the pool.  If no
// allocation is available, then it returns a NULL.  The calling process then
// would normally allocate more memory, and assign it to the pool.  Since in
// most cases, the calling process would normally want to then work with the
// memory straight away, it will be added to the system in the 'used' pool.
void mempool_assign(mempool_t *pool, void *ptr, unsigned int size)
{
	mempool_entry_t *tmp;
	
	assert(pool);
	assert(ptr);
	assert(size > 0);

	tmp = (mempool_entry_t *)	malloc(sizeof(mempool_entry_t));
	assert(tmp);
	tmp->data = ptr;
	tmp->size = size;
	tmp->prev = NULL;
	tmp->next = pool->used;
}


//-----------------------------------------------------------------------------
// Sometimes when a chunk of memory is retrieved from the pool, it needs to be
// removed from the pool and used (or controlled) by something else.   In
// these cases, it is assumed that the pointer is already in the 'used' list,
// so the entry will be found and removed, without de-allocating the pointer.
// It is recommended that the calling function perform a resize on the pointer
// though, because it might have more memory allocated to it, than was
// requested.
void mempool_release(mempool_t *pool, void *ptr)
{
	mempool_entry_t *tmp;
	
	assert(pool);
	assert(ptr);
	assert(pool->used);

	tmp = pool->used;
	while(tmp) {
		assert(tmp->data);
		if (tmp->data == ptr) {
			assert(tmp->size > 0);
			if (tmp == pool->used) pool->used = tmp->next;
			if (tmp->prev) tmp->prev->next = tmp->next;
			if (tmp->next) tmp->next->prev = tmp->prev;
			free(tmp);
			tmp = NULL;
		}
		else {
			tmp = tmp->next;
		}
	}
}



