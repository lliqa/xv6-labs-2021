#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "param.h"

void*
malloc(uint size); 
void
free(void* ptr);  

void
heap_demo(void)
{
  void* p1 = malloc(64);
  printf("malloc 64 => %p\n", p1);
  void* p2 = malloc(128);
  printf("malloc 128 => %p\n", p2);
  free(p1);
  printf("free %p\n", p1);
  free(p2);
  printf("free %p\n", p2);
}