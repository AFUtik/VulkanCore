#pragma once
/*

#include <atomic>

using u32 = std::uint32_t;

struct VkResourceManager;

template<typename T>
struct StaticHandle 
{
    u32 index;
};

struct ResourceBlock {
    std::atomic<uint32_t> refCount  {0};
    std::atomic<uint32_t> weakCount {0}; 
    u32                   index = ~0u;
    bool                  alive = false;

    void (*destroy)(VkResourceManager*, ResourceBlock*) = nullptr;
};

template<typename T> class WeakHandle;

template<typename T>
class Handle {
public:
    Handle() = default;

    Handle(const Handle& o) : block_(o.block_), manager_(o.manager_) 
    {
        if (block_) block_->refCount.fetch_add(1, std::memory_order_relaxed);
    }

    Handle(Handle&& o) noexcept : block_(o.block_), manager_(o.manager_) 
    {
        o.block_ = nullptr; o.manager_ = nullptr;
    }

    Handle& operator=(Handle o) noexcept { Swap(o); return *this; }
    ~Handle() { Release(); }

    T*   operator->() const { return Get(); }
    T&   operator* () const { return *Get(); }
    bool IsValid()    const { return block_ && block_->alive; }
    explicit operator bool() const { return IsValid(); }

    WeakHandle<T> GetWeak() const { return WeakHandle<T>(*this); }
private:
    friend struct VkResourceManager;
    friend class  WeakHandle<T>;

    ResourceBlock*     block_   = nullptr;
    VkResourceManager* manager_ = nullptr;

    T* Get() const;

    void Release() 
    {
        if (!block_) return;
        if (block_->refCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            block_->destroy(manager_, block_);
            if (block_->weakCount.fetch_sub(1, std::memory_order_acq_rel) == 1) delete block_;
        }
        block_ = nullptr; manager_ = nullptr;
    }

    void Swap(Handle& o) noexcept 
    {
        std::swap(block_,   o.block_);
        std::swap(manager_, o.manager_);
    }
};

template<typename T>
class WeakHandle {
public:
    WeakHandle() = default;

    explicit WeakHandle(const Handle<T>& h) : block_(h.block_), manager_(h.manager_) {
        if (block_) block_->weakCount.fetch_add(1, std::memory_order_relaxed);
    }
    
    WeakHandle(const WeakHandle& o) : block_(o.block_), manager_(o.manager_) 
    {
        if (block_) block_->weakCount.fetch_add(1, std::memory_order_relaxed);
    }

    WeakHandle(WeakHandle&& o) noexcept : block_(o.block_), manager_(o.manager_) 
    {
        o.block_ = nullptr; o.manager_ = nullptr;
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
                h.block_ = block_; h.manager_ = manager_;
                return h;
            }
        }
        return {};
    }

private:
    friend class Handle<T>;

    ResourceBlock*     block_   = nullptr;
    VkResourceManager* manager_ = nullptr;

    void Release() {
        if (!block_) return;
        if (block_->weakCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
            delete block_;
        block_ = nullptr; manager_ = nullptr;
    }

    void Swap(WeakHandle& o) noexcept {
        std::swap(block_,   o.block_);
        std::swap(manager_, o.manager_);
    }
};

*/