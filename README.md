# Description:
### Implement pool-based allocator which respects provided interface

* Pool allocator internally should use chunks of a same size
  * *Chunk size and amount of chunks passed in constructor.*
  
* Memory allocation for pool happens in construction, deallocation – in destructor 
  *_no dynamic memory allocation in runtime_
  
* Each allocation can occupy one or more “free” chunks
  * *If allocation impossible then nullptr returned*
  
* Each free must check if pointer allocated from the pool or not

* If pool doesn’t “own” pointer then nothing happened

* Clear makes all chunks “free”.

## Image explanation
![alt text](https://github.com/bohdanpc/Pool-based_Allocator/edit/master/PoolAllocator_png.png)

Whenever an allocation request is made – simply grab the next free element oﬀ  the free list and return it
When an element is freed, we simply tack it back onto the free list
Both allocations and frees are O(1) operations, since each involves only a couple of pointer manipulations, no mater how many elements are currently free
Performance for allocator really depends on internal data structures to be used in implementation (measurements presented for linked lists)
