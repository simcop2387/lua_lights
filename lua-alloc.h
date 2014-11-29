#ifndef __LUALIGHTS_LUA_ALLOC_H
#define __LUALIGHTS_LUA_ALLOC_H

// 24K seems to be an ok amount for now.  Might need to expand it
#define LL_ARENA_SIZE 24576


typedef size_t malloc_size_t;

typedef struct malloc_chunk
{
    /*          ------------------
     *   chunk->| size (4 bytes) |
     *          ------------------
     *          | Padding for    |
     *          | alignment      |
     *          | holding neg    |
     *          | offset to size |
     *          ------------------
     * mem_ptr->| point to next  |
     *          | free when freed|
     *          | or data load   |
     *          | when allocated |
     *          ------------------
     */
    /* size of the allocated payload area, including size before
       CHUNK_OFFSET */
    long size;

    /* since here, the memory is either the next free block, or data load */
    struct malloc_chunk * next;
}chunk;

/* Copied from malloc.h */
struct mallinfo
{
  size_t arena;    /* total space allocated from system */
  size_t ordblks;  /* number of non-inuse chunks */
  size_t smblks;   /* unused -- always zero */
  size_t hblks;    /* number of mmapped regions */
  size_t hblkhd;   /* total space in mmapped regions */
  size_t usmblks;  /* unused -- always zero */
  size_t fsmblks;  /* unused -- always zero */
  size_t uordblks; /* total allocated space */
  size_t fordblks; /* total non-inuse space */
  size_t keepcost; /* top-most, releasable (via malloc_trim) space */
};

/* Forward function declarations */
extern void * ll_malloc(malloc_size_t);
extern void ll_free (void * free_p);
extern void * ll_calloc(malloc_size_t n, malloc_size_t elem);
extern struct mallinfo ll_mallinfo();
extern malloc_size_t ll_malloc_usable_size(void * ptr);
extern void * ll_realloc(void * ptr, malloc_size_t size);
extern void ll_clean_arena();

#endif