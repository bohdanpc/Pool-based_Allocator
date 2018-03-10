#pragma once
#include <cstddef>
#include <iostream>

class PoolAllocator
{
	struct MemoryChunck {
		std::size_t dataSize;
		std::size_t usedSize;
        MemoryChunck* nextChunck;
		char* data;
	};

public:
	explicit PoolAllocator(std::size_t chunckSize, std::size_t chuncksCount);
    ~PoolAllocator();
	void* allocate(std::size_t size);
	void free(void* pointer);
	void clear();
    void showMemoryDump(std::ostream& out = std::cout);

private:
    void mapFreeListToMemBlock();
    MemoryChunck* initMemoryChunck(MemoryChunck* memChunck);
    MemoryChunck* skipMemoryChuncks(MemoryChunck* memChunck, unsigned int& checkedChuncksCounter);
    MemoryChunck* skipCheckMemoryChuncks(MemoryChunck* memChunck, std::size_t usedMemory, unsigned int& checkedChuncksCounter, bool& isEnoughMemory);
    MemoryChunck* m_firstChunck;
    MemoryChunck* m_lastChunck;
    MemoryChunck* m_currentChunck;

    const std::size_t m_chunckSize;
    const std::size_t m_chuncksCount;

    char* m_memoryBlock;
    MemoryChunck * m_freeList;
};
