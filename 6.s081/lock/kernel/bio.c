// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

struct {
  struct buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  struct buf head[NBUCKET];
  struct spinlock bucketlock[NBUCKET];
} bcache;

void
binit(void)
{
  struct buf *b;

  // Create linked list of buffers
  for (int i = 0; i < NBUCKET; ++i){
    bcache.head[i].prev = &bcache.head[i];
    bcache.head[i].next = &bcache.head[i];    
    initlock(&bcache.bucketlock[i], "bcache");
  }    

  for(int i = 0; i < NBUF; ++i){
    b = bcache.buf + i;
    b->next = bcache.head[i % NBUCKET].next;
    b->prev = &bcache.head[i % NBUCKET];
    initsleeplock(&b->lock, "buffer");
    bcache.head[i % NBUCKET].next->prev = b;
    bcache.head[i % NBUCKET].next = b;
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;
  int key = blockno % NBUCKET;

  // Is the block already cached?
  acquire(&bcache.bucketlock[key]);
  for(b = bcache.head[key].next; b != &bcache.head[key]; b = b->next){
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      b->timestamp = ticks;
      release(&bcache.bucketlock[key]);
      acquiresleep(&b->lock);
      //printf("1");
      return b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  uint timestamp = 0xffffffff;
  struct buf* final = 0;
  int locked = 0;

  for (int i = 0; i < NBUCKET; ++i){
    if (i != key && !bcache.bucketlock[i].locked){
      acquire(&bcache.bucketlock[i]);
      locked = 1;
    }
    else
      continue;
    
    for(b = bcache.head[i].prev; b != &bcache.head[i]; b = b->prev){
      if(b->refcnt == 0 && b->timestamp < timestamp){
        if (final){
          // Put the former one back to the bucket where we fetch it out.
          int j = final->blockno % NBUCKET;
          if (j != i && j != key)
            acquire(&bcache.bucketlock[j]);
          final->next = bcache.head[j].next;
          final->prev = &bcache.head[j];
          bcache.head[j].next->prev = final;
          bcache.head[j].next = final;
          if (j != i && j != key)
            release(&bcache.bucketlock[j]);
        }
        final = b;
        timestamp = b->timestamp;
        b->next->prev = b->prev;
        b->prev->next = b->next;
      }
    }
    if (locked){
      release(&bcache.bucketlock[i]);
      locked = 0;
    }
  }

  if (final){    
    final->next = bcache.head[key].next;
    final->prev = &bcache.head[key];
    bcache.head[key].next->prev = final;
    bcache.head[key].next = final;

    final->dev = dev;
    final->blockno = blockno;
    final->valid = 0;
    final->refcnt = 1;
    final->timestamp = ticks;

    release(&bcache.bucketlock[key]);
    acquiresleep(&final->lock);
    //printf("2");
    return final;
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  acquire(&bcache.bucketlock[b->blockno % NBUCKET]);
  b->refcnt--;
  b->timestamp = ticks;
  release(&bcache.bucketlock[b->blockno % NBUCKET]);
}

void
bpin(struct buf *b) {
  acquire(&bcache.bucketlock[b->blockno % NBUCKET]);
  b->refcnt++;
  release(&bcache.bucketlock[b->blockno % NBUCKET]);
}

void
bunpin(struct buf *b) {
  acquire(&bcache.bucketlock[b->blockno % NBUCKET]);
  b->refcnt--;
  release(&bcache.bucketlock[b->blockno % NBUCKET]);
}


