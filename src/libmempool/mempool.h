#ifndef __MEMPOOL_H
#define __MEMPOOL_H

// mempool does not create new memory objects as needed.  Ie, it does not automatically expand.  When there are no available objects in the pool, then new ones will need to  be explicitly added.


typedef struct __mempool_entry_t {
	void *data;
	unsigned int size;
	struct __mempool_entry_t *prev, *next;
} mempool_entry_t;


typedef struct {
	mempool_entry_t *ready, *used;
	short int expand;
} mempool_t;


void mempool_init(mempool_t *pool);
void mempool_free(mempool_t *pool);
void mempool_autoexpand(mempool_t *pool);

void * mempool_get(mempool_t *pool, unsigned int amount);
void mempool_return(mempool_t *pool, void *ptr);

void mempool_assign(mempool_t *pool, void *ptr, unsigned int size);
void mempool_release(mempool_t *pool, void *ptr);

unsigned int mempool_active_count(mempool_t *pool);
unsigned int mempool_inactive_count(mempool_t *pool);

void * mempool_peek(mempool_t *pool);

#endif

