#include <assert.h>
#include "PoolAllocator.h"

PoolAllocator::PoolAllocator(std::size_t chunckSize, std::size_t chuncksCount) :
    m_chuncksCount(chuncksCount), m_chunckSize(chunckSize)
{
    try 
    {
        m_memoryBlock = new char[chuncksCount * chunckSize];
        m_freeList = new MemoryChunck[m_chuncksCount];
    }
    catch (std::bad_alloc&)
    {
        assert(false && "Error: couldn't allocate enough memory to hold memory block\n");
    }
    mapFreeListToMemBlock();
}


PoolAllocator::~PoolAllocator()
{
    delete[] m_memoryBlock;
    delete[] m_freeList;
}


PoolAllocator::MemoryChunck* PoolAllocator::initMemoryChunck(MemoryChunck* memChunck)
{
    memChunck->data = nullptr;
    memChunck->dataSize = 0;
    memChunck->usedSize = 0;
    memChunck->nextChunck = nullptr;
    return memChunck;
}


void PoolAllocator::mapFreeListToMemBlock()
{
    for (std::size_t chunckIdx = 0; chunckIdx < m_chuncksCount; chunckIdx++)
    {
        if (!m_firstChunck)
        {
            m_firstChunck = initMemoryChunck(&m_freeList[chunckIdx]);
            m_lastChunck = m_firstChunck;
            m_currentChunck = m_lastChunck;
        }
        else
        {
            m_lastChunck->nextChunck = initMemoryChunck(&(m_freeList[chunckIdx]));
            m_lastChunck = m_lastChunck->nextChunck;
        }

        std::size_t currMemoryBlockOffset = chunckIdx * m_chunckSize;
        m_lastChunck->data = &(m_memoryBlock[currMemoryBlockOffset]);
        m_lastChunck->dataSize = m_chuncksCount * m_chunckSize - currMemoryBlockOffset;
    }
}


PoolAllocator::MemoryChunck* PoolAllocator::skipMemoryChuncks(MemoryChunck* memChunck, unsigned int& checkedChuncksCounter)
{
    std::size_t chuncksToSkip = memChunck->usedSize / m_chunckSize;
    if (0 == chuncksToSkip || memChunck->usedSize % m_chunckSize != 0)
    {
        chuncksToSkip++;
    }

    for (std::size_t j = 0; j < chuncksToSkip; j++)
    {
        if (memChunck)
        {
            checkedChuncksCounter++;
            memChunck = memChunck->nextChunck;
        }
        else
        {
            assert(false && "Error: memory pool is broken\n");
        }
    }
    return memChunck;
}


/*
 * Check if there's enough memory space to allocate 
 * and jump to next memoryChunk
 */
PoolAllocator::MemoryChunck* PoolAllocator::skipCheckMemoryChuncks(MemoryChunck* memChunck, std::size_t usedSize, unsigned int& checkedChuncksCounter, bool& isEnoughMemory)
{
    std::size_t chuncksToSkip = usedSize / m_chunckSize;
    if (0 == chuncksToSkip || usedSize % m_chunckSize != 0)
    {
        chuncksToSkip++;
    }
    for (std::size_t j = 0; j < chuncksToSkip; j++)
    {
        checkedChuncksCounter++;
        if (memChunck)
        {
            if (memChunck->usedSize != 0)
            {
                isEnoughMemory = false;
                return memChunck->nextChunck;
            }
            memChunck = memChunck->nextChunck;
        }
        else
        {
            assert(false && "Error: memory pool is broken\n");
        }
    }

    isEnoughMemory = true;
    return memChunck;
}


void* PoolAllocator::allocate(std::size_t memorySize)
{
    MemoryChunck* currMemoryChunck = m_currentChunck;
    bool isEnoughMemory = false;

    std::size_t checkedChuncksCounter = 0;
    while (checkedChuncksCounter < m_chuncksCount)
    {
        if (currMemoryChunck && currMemoryChunck->dataSize >= memorySize)
        {
            if (currMemoryChunck->usedSize == 0)
            {
                MemoryChunck* tmpChunck = skipCheckMemoryChuncks(currMemoryChunck, memorySize, checkedChuncksCounter, isEnoughMemory);
                if (isEnoughMemory)
                {
                    m_currentChunck = tmpChunck;
                    currMemoryChunck->usedSize = memorySize;
                    return static_cast<void*>(currMemoryChunck->data);
                }
                else
                {
                    currMemoryChunck = tmpChunck;
                }
            }
            currMemoryChunck = skipMemoryChuncks(currMemoryChunck, checkedChuncksCounter);
        }
        else
        {
            //then there's no enough memory to allocate in memoryBlock
            if (currMemoryChunck == m_firstChunck)
            {
                break;
            }
            currMemoryChunck = m_firstChunck;
        }
    }
    return nullptr;
}


/*
 * find according pointer in free list 
 * additionaly check if ptr belongs to freeList
 */
void PoolAllocator::free(void* pointer)
{
    MemoryChunck* currMemChunck = m_firstChunck;
    while (currMemChunck && currMemChunck->data != (char *)pointer)
    {
        currMemChunck = currMemChunck->nextChunck;
    }

    if (currMemChunck)
    {
        currMemChunck->usedSize = 0;
    }
}


void PoolAllocator::clear()
{
    MemoryChunck* currChunck = m_firstChunck;
    while (currChunck)
    {
        if (currChunck->usedSize)
        {
            currChunck->usedSize = 0;
        }
    }
}

void PoolAllocator::showMemoryDump(std::ostream& out)
{
    out << "---General pool-based allocator information---" << std::endl;
    
    unsigned int generalChuncksCount = 0;
    unsigned int freeChuncksCount = 0;
    unsigned int occupiedChuncksCount = 0;
    unsigned int maxContiguousBlockMemory = 0;
    unsigned int contigiousBlockCounter = 0;
    MemoryChunck* currChunck = m_firstChunck;

    while (currChunck)
    {
        if (currChunck->usedSize == 0)
        {
            contigiousBlockCounter++;
            freeChuncksCount++;
            if (contigiousBlockCounter > maxContiguousBlockMemory)
            {
                maxContiguousBlockMemory = contigiousBlockCounter;
            }
            currChunck = currChunck->nextChunck;
            generalChuncksCount++;
        }
        else
        {
            currChunck = skipMemoryChuncks(currChunck, generalChuncksCount);
            contigiousBlockCounter = 0;
        }
    }
    occupiedChuncksCount = m_chuncksCount - freeChuncksCount;

    out << "ChuncksCount: " << generalChuncksCount << std::endl;
    out << "Free chuncks count: " << freeChuncksCount << std::endl;
    out << "Occupied chuncks count: " << occupiedChuncksCount << std::endl;
    out << "Amount of free memory: " << freeChuncksCount * m_chunckSize << std::endl;
    out << "Amount of occupied memory: " << occupiedChuncksCount * m_chunckSize << std::endl;
    out << "Maximum contigious memory block: " << maxContiguousBlockMemory << std::endl << std::endl;
}