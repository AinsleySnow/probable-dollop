// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void* pa_start, void* pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run
{
  struct run* next;
};

struct
{
  struct spinlock lock;
  struct run* freelist;
} kmem;

struct
{
  struct spinlock lock;
  uint16 refcount[32768];
} kref;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&kref.lock, "kref");
  memset(kref.refcount, 0, sizeof(kref.refcount));
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void* pa_start, void* pa_end)
{
  char* p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for (; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void* pa)
{
  struct run* r;

  if (((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  acquire(&kref.lock);
  if (kref.refcount[((uint64)pa >> 12) - 524288] == 0 ||
    --kref.refcount[((uint64)pa >> 12) - 524288] == 0)
  {
    release(&kref.lock);
    // Fill with junk to catch dangling refs.
    memset(pa, 1, PGSIZE);

    r = (struct run*)pa;

    acquire(&kmem.lock);
    r->next = kmem.freelist;
    kmem.freelist = r;
    release(&kmem.lock);
  }
  else
    release(&kref.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void*
kalloc(void)
{
  struct run* r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if (r)
  {
    kmem.freelist = r->next;
    addrefcount((uint64)r);
  }
  release(&kmem.lock);

  if (r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

void addrefcount(uint64 pa)
{
  acquire(&kref.lock);
  kref.refcount[(pa >> 12) - 524288]++;
  release(&kref.lock);
}

int knewpg(uint64 vaddr, pagetable_t pgtbl)
{
  if(vaddr >= MAXVA)
    return -1;
  
  pte_t* pte = walk(pgtbl, vaddr, 0);
  if (pte == 0)
    return -1;
  uint64 pa = PTE2PA(*pte);

  uint64 flags = PTE_FLAGS(*pte);
  flags |= PTE_W;
  flags &= ~PTE_RSW;

  char* mem = kalloc();
  if (mem == 0)
    return -1;

  memmove(mem, (char*)pa, PGSIZE);
  kfree((void*)pa);
  *pte = PA2PTE(mem) | flags;
  return 0;
}
