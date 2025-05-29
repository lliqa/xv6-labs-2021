// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

#define HEAP_SIZE (8 * 1024 * 1024) // 8MB


extern char end[]; // first address after kernel.
                   // defined by kernel.ld
char* heap_start;
char* heap_end;

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

typedef struct block {
    uint size;             // 数据部分大小（不包含头部）
    struct block *next;
    int free;              // 是否空闲
} block_t;

#define BLOCK_SIZE sizeof(block_t)


void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP - HEAP_SIZE);

  heap_end = (char*)PHYSTOP;
  heap_start = heap_end - HEAP_SIZE;

  block_t *p = (block_t *)heap_start;
  p->size = HEAP_SIZE - BLOCK_SIZE;
  p->next = 0;
  p->free = 1;
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP - HEAP_SIZE)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}


void getFreemem(uint64 *freemem) {
    *freemem = 0;

    struct run *r;

    acquire(&kmem.lock);
    r = kmem.freelist;
    while (r) {
        *freemem += PGSIZE;
        r = r->next;
    }
    release(&kmem.lock);
}

void* malloc(uint size) {
    block_t *curr = (block_t *)heap_start;
    
    while (curr) {
        if (curr->free && curr->size >= size) {
            // 若空闲块足够大，则分配
            if (curr->size >= size + BLOCK_SIZE) {
                // 拆分
                block_t *new_block = (block_t*)((char*)curr + BLOCK_SIZE + size);
                new_block->size = curr->size - size - BLOCK_SIZE;
                new_block->next = curr->next;
                new_block->free = 1;

                curr->next = new_block;
                curr->size = size;
            }
            curr->free = 0;
            return (char*)curr + BLOCK_SIZE;
        }
        curr = curr->next;
    }
    return 0; // 分配失败
}


void free(void *ptr) {
    if (!ptr)
        return;

    block_t *blk = (block_t *)((char*)ptr - BLOCK_SIZE);
    blk->free = 1;
}