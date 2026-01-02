#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <vector>
#include <cstddef>
#include <utility>

template<typename T, size_t BlockSize = 4096>
class MemoryPool {
private:
    struct Block {
        char data[BlockSize * sizeof(T)];
    };

    std::vector<Block*> blocks;
    std::vector<T*> freeList;
    size_t currentBlockIndex;
    
public:
    MemoryPool() : currentBlockIndex(BlockSize) {} // Start full to force first alloc

    ~MemoryPool() {
        for (Block* block : blocks) {
            delete block;
        }
    }

    // Disable copy
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

    template<typename... Args>
    T* allocate(Args&&... args) {
        if (!freeList.empty()) {
            T* ptr = freeList.back();
            freeList.pop_back();
            new (ptr) T(std::forward<Args>(args)...); // Construct in-place
            return ptr;
        }

        if (currentBlockIndex >= BlockSize) {
            blocks.push_back(new Block());
            currentBlockIndex = 0;
        }

        T* ptr = reinterpret_cast<T*>(blocks.back()->data + currentBlockIndex * sizeof(T));
        currentBlockIndex++;
        
        new (ptr) T(std::forward<Args>(args)...);
        return ptr;
    }

    void deallocate(T* ptr) {
        if (ptr) {
            ptr->~T(); // Destruct
            freeList.push_back(ptr);
        }
    }
    
    // Optional: Reset pool without freeing memory (fast clear)
    void clear() {
        freeList.clear();
        // Naive clean: we can't easily reuse blocks linearly if we mixed allocs, 
        // but for a strict pool we could just reset indices if we knew everything was freed.
        // For safety with mixed usage (which shouldn't happen in a heap clear usually but might),
        // we'll just clear everything.
        for (Block* block : blocks) {
            delete block;
        }
        blocks.clear();
        currentBlockIndex = BlockSize;
    }
};

#endif // MEMORY_POOL_H
