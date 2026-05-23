#pragma once

#include <atomic>
#include <cstdint>
#include <utility>
#include <cassert>

#ifndef NDEBUG
#include <iostream>
#endif

namespace gfx 
{

using u32 = std::uint32_t;
using u8  = std::uint8_t;

struct ResourceManager;

struct ResourceBlockBase {
    void* object  = nullptr;
    void* manager = nullptr;
    std::atomic<u32> refCount  {1};
    std::atomic<u32> weakCount {0};
    u32              index = ~0u;
    bool             alive = false;
    void (*destroyFn)(ResourceBlockBase*) = nullptr;
};

template<typename T>
struct Handle {
    Handle() = default;

    Handle(const Handle& o) : block_(o.block_)
    {
        if (block_) block_->refCount.fetch_add(1, std::memory_order_relaxed);
    }

    Handle(Handle&& o) noexcept : block_(o.block_)
    {
        o.block_ = nullptr;
    }

    Handle& operator=(Handle o) noexcept { Swap(o); return *this; }
    ~Handle() { Release(); }

    T*   operator->() const { return Get(); }
    T&   operator* () const { return *Get(); }
    bool IsValid()    const { return block_ && block_->alive; }
    explicit operator bool() const { return IsValid(); }
    
    T* Get() const
    {
        assert(block_);
        assert(block_->alive);

        return reinterpret_cast<T*>(block_->object);
    }

    uint32_t index() const
    {
        assert(block_);
        assert(block_->index != ~0u);

        return block_->index;
    }
private:
    friend struct ResourceManager;

    ResourceBlockBase* block_ = nullptr;

    void Release() 
    {
        if (!block_) return;
        
        if (block_->refCount.fetch_sub(1, std::memory_order_acq_rel) == 1) block_->destroyFn(block_);

        block_ = nullptr;
    }

    void Swap(Handle& o) noexcept 
    {
        std::swap(block_,   o.block_);
    }
};

/*
template<typename T>
struct WeakHandle {
    WeakHandle() = default;

    explicit WeakHandle(const Handle<T>& h) : block_(h.block_) {
        if (block_) block_->weakCount.fetch_add(1, std::memory_order_relaxed);
    }
    
    WeakHandle(const WeakHandle& o) : block_(o.block_)
    {
        if (block_) block_->weakCount.fetch_add(1, std::memory_order_relaxed);
    }

    WeakHandle(WeakHandle&& o) noexcept : block_(o.block_)
    {
        o.block_ = nullptr;
    }

    WeakHandle& operator=(WeakHandle o) noexcept { Swap(o); return *this; }
    ~WeakHandle() { Release(); }

    bool IsValid() const { return block_ && block_->alive; }

    Handle<T> Lock() const {
        if (!block_) return {};
        uint32_t cur = block_->refCount.load(std::memory_order_relaxed);
        while (cur > 0) {
            if (block_->refCount.compare_exchange_weak(
                    cur, cur + 1,
                    std::memory_order_acq_rel,
                    std::memory_order_relaxed)) {
                Handle<T> h;
                h.block_ = block_;
                return h;
            }
        }
        return {};
    }

private:
    friend struct Handle<T>;

    ResourceBlockBase*   block_   = nullptr;

    void Release() {
        if (!block_) return;

        block_ = nullptr;
    }

    void Swap(WeakHandle& o) noexcept 
    {
        std::swap(block_,   o.block_);
    }
};
*/

}