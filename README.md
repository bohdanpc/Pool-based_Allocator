# Pool-based_Allocator
Description:
Implement pool-based allocator which respects provided interface

* Pool allocator internally should use chunks of a same size
  * *Chunk size and amount of chunks passed in constructor.*
  
* Memory allocation for pool happens in construction, deallocation – in destructor 
  *_no dynamic memory allocation in runtime_
  
* Each allocation can occupy one or more “free” chunks
  * *If allocation impossible then nullptr returned*
  
* Each free must check if pointer allocated from the pool or not

* If pool doesn’t “own” pointer then nothing happened

* Clear makes all chunks “free”.
