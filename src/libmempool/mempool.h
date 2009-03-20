#ifndef __MEMPOOL_H
#define __MEMPOOL_H

// mempool does not create new memory objects as needed.  Ie, it does not automatically expand.  When there are no available objects in the pool, then new ones will need to  be explicitly added.

struct struct mempool_entry_t;
typedef struct {
	void *data;
	unsigned int size;
	mempool_entry_t *prev, *next;
} mempool_entry_t;


typedef struct {
	mempool_entry_t *ready, *used;
} mempool_t;


void mempool_init(mempool_t *pool);
void mempool_free(mempool_t *pool);

void * mempool_get(mempool_t *pool, unsigned int amount);
void expbuf_pool_return(mempool_t *pool, void *ptr);

void mempool_assign(mempool_t *pool, void *ptr, unsigned int size);
void mempool_release(mempool_t *pool, void *ptr);

#endif

