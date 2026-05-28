#pragma once

#include <atomic>
#include <cstdint>
#include <utility>
#include <cassert>

namespace gfx 
{

using u32 = std::uint32_t;
using u8  = std::uint8_t;

struct ResourceBlockBase {
    void* object  = nullptr;
    void* manager = nullptr;
    std::atomic<u32> refCount  {1};
    std::atomic<u32> weakCount {0};
    u32              index = ~0u;
    bool             alive = false;
    void (*destroyFn)(ResourceBlockBase*) = nullptr;
};

struct unknown_type {};

template<typename T = unknown_type>
struct Handle {
    Handle() = default;

    Handle(const Handle& o) noexcept : block_(o.block_)
    {
        if (block_)
            block_->refCount.fetch_add(1, std::memory_order_relaxed);
    }

    Handle(Handle&& o) noexcept : block_(o.block_)
    {
        o.block_ = nullptr;
    }

    template<typename C>
    requires std::is_convertible_v<C*, T*>
    Handle(const Handle<C>& o) noexcept : block_(o.block_)
    {
        if (block_)
            block_->refCount.fetch_add(1, std::memory_order_relaxed);
    }

    template<typename C>
    requires std::is_convertible_v<C*, T*>
    Handle(Handle<C>&& o) noexcept : block_(o.block_)
    {
        o.block_ = nullptr;
    }

    Handle& operator=(const Handle& o) noexcept
    {
        Handle tmp(o);
        Swap(tmp); 
        return *this;
    }

    Handle& operator=(Handle&& o) noexcept
    {
        if (this != &o)
        {
            Release();
            block_   = o.block_;
            o.block_ = nullptr;
        }
        return *this;
    }

    template<typename C>
    requires std::is_convertible_v<C*, T*>
    Handle& operator=(const Handle<C>& o) noexcept
    {
        Handle tmp(o);
        Swap(tmp);
        return *this;
    }

    template<typename C>
    requires std::is_convertible_v<C*, T*>
    Handle& operator=(Handle<C>&& o) noexcept
    {
        Release();
        block_   = o.block_;
        o.block_ = nullptr;
        return *this;
    }

    ~Handle() { Release(); }

    T*   operator->() const { return Get(); }
    T&   operator* () const { return *Get(); }
    bool IsValid()    const { return block_ && block_->alive; }
    explicit operator bool() const { return IsValid(); }

    template<typename C>
    Handle<C> Cast() const { return Handle<C>(*this); }
    
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
    
    template<typename>
    friend class Handle;

    template<typename, size_t>
    friend struct ResourceManager;

    ResourceBlockBase* block_ = nullptr;

    void Release() 
    {
        if (!block_) return;
        
        if (block_->refCount.fetch_sub(1, std::memory_order_acq_rel) == 1) 
            block_->destroyFn(block_);

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