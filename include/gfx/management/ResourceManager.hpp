#pragma once

#include "Handle.hpp"

#include <memory>
#include <new>
#include <vector>
#include <cstring>

namespace gfx
{

template<typename T, size_t BLOCK_SIZE = 512>
struct ResourceManager
{
    ~ResourceManager()
    {
        for(int i = 0; i < slots.size(); i++)
        {
            if(!slots[i].alive) continue;

            auto& block = blocks_.blocks[blockIndex(i)];
            auto resBlock = reinterpret_cast<ResourceBlockBase*>(block[i % BLOCK_SIZE]);
            resBlock->destroyFn(resBlock);
        }
    }

    static constexpr uint32_t kNone = ~0u;

    struct Slot 
    {
        uint32_t next  = kNone;
        bool     alive = false;
    };

    template<typename C>
    struct Array
    {
        struct AlignedDeleter 
        {
            std::size_t align = alignof(std::max_align_t);
    
            void operator()(void* p) const noexcept {
                ::operator delete[](p, std::align_val_t(align));
            }
        };
    
        struct Block 
        {
            Array* parent = nullptr;
            std::unique_ptr<std::byte[], AlignedDeleter> data;
    
            Block(Array* parent) : parent(parent), data(nullptr, AlignedDeleter{alignof(C)})
            {
                const std::size_t data_size = BLOCK_SIZE * sizeof(C);
                void* raw = ::operator new[](data_size, std::align_val_t(alignof(C)));
                data.reset(static_cast<std::byte*>(raw));
                std::memset(data.get(), 0, data_size);
            }
    
            void* operator[](std::size_t i) const 
            {
                assert(i < BLOCK_SIZE);
                return reinterpret_cast<void*>(data.get() + (sizeof(C) * i));
            }
        };

        std::vector<Block> blocks;

        Array() = default;

        void resize(std::size_t n) 
        {
            const std::size_t needed_blocks = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;
            while (blocks.size() < needed_blocks) {
                blocks.emplace_back(this);
            }
        }

        void* operator[](std::size_t i) const {
            return blocks[i / BLOCK_SIZE][i % BLOCK_SIZE];
        }
    };

    template<typename... Args>
    Handle<T> Create(Args&&... args)
    {
        u32 index = emplace();
        new (std::launder(reinterpret_cast<T*>(objects_[index]))) T(std::forward<Args>(args)...);
        new (std::launder(reinterpret_cast<ResourceBlockBase*>(blocks_[index]))) ResourceBlockBase();

        auto block = static_cast<ResourceBlockBase*>(blocks_[index]);
        block->object  = objects_[index];
        block->manager = this;
        block->index = index;
        block->alive = true;
        block->destroyFn = [](ResourceBlockBase* b) 
        {
            static_cast<ResourceManager<T, BLOCK_SIZE>*>(b->manager)->free(b->index);

            std::destroy_at(static_cast<T*>(b->object));
            b->alive = false;
        };
        
        Handle<T> h;
        h.block_ = block;
        return h;
    }
    
    Array<T> objects_;
    Array<ResourceBlockBase> blocks_;
    std::vector<Slot> slots;
private:
    static constexpr uint32_t blockIndex(uint32_t idx) noexcept 
    {
        return idx / BLOCK_SIZE;
    }

    void ensureSlotExists(uint32_t idx) 
    {
        idx+=1;

        slots.resize(idx);
        blocks_.resize(idx);
        objects_.resize(idx);
    }

    uint32_t emplace()
    {
        uint32_t idx = kNone;

        if (freeHead_ != kNone) {
            idx = freeHead_;
            Slot& s = slots[idx];
            freeHead_ = s.next;

            assert(!s.alive);
            s.alive = true;

            ++liveCount_;
            return idx;
        }

        idx = slotCount_;
        ensureSlotExists(idx);
        
        ++slotCount_;
        ++liveCount_;

        Slot& s = slots[idx];
        assert(!s.alive);

        s.alive = true;
        s.next  = kNone;

        return idx;
    }

    void free(uint32_t idx) 
    {
        Slot& s = slots[idx];

        s.alive = false;

        s.next = freeHead_;
        freeHead_ = idx;

        --liveCount_;
    }

    uint32_t slotCount_  = 0; 
    uint32_t liveCount_  = 0;
    uint32_t freeHead_   = kNone;

    
};

} // namespace gfx