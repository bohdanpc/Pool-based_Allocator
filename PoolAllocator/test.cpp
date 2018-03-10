#include "PoolAllocator.h"
#include <iostream>
#include <fstream>
#include <assert.h>

int main(int argc, char**argv) {
    
    const std::size_t ChuncksCount = 5;
    const std::size_t ChunckSize = 100;
    const std::size_t MaxBufferSize = ChuncksCount * ChunckSize;

    PoolAllocator allocator(ChunckSize, ChuncksCount);

    char* buffer = (char*) allocator.allocate(MaxBufferSize);

    if (argc >= 2)
    {
        std::ifstream fin(argv[1], std::ios::in | std::ios::binary);
        if (!fin.is_open())
        {
            std::cerr << "Error: File \"" << argv[1] << "\" was not opened" << std::endl;
        }
        else
        {
            while (fin.getline(buffer, MaxBufferSize, '\n'))
            {
                std::cout << buffer << std::endl;
            }
        }
    }
    else
    {
        std::cerr << "Error: Invalid number of arguments" << std::endl;
    }

    allocator.showMemoryDump();
	return 0;
}
