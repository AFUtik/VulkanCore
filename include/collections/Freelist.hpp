#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>
#include <vector>

template<typename T, std::size_t BLOCK_SIZE = 512>
class FreeList {
public:
    static constexpr uint32_t kNone = ~0u;
    
    T* operator[](size_t idx) {
        assert(idx < slotCount_);
        return blocks_[blockIndex(idx)][slotIndexInBlock(idx)].value();
    }
private:
    struct Slot {
        alignas(T) std::byte storage[sizeof(T)];

        uint32_t next       = kNone;
        bool     alive      = false;

        T* value() {
            return std::launder(reinterpret_cast<T*>(storage));
        }

        const T* value() const {
            return std::launder(reinterpret_cast<const T*>(storage));
        }
    };

    std::vector<std::unique_ptr<Slot[]>> blocks_;

    uint32_t slotCount_  = 0; 
    uint32_t liveCount_  = 0;
    uint32_t freeHead_   = kNone;

    static constexpr uint32_t blockIndex(uint32_t idx) noexcept {
        return idx / BLOCK_SIZE;
    }

    static constexpr uint32_t slotIndexInBlock(uint32_t idx) noexcept {
        return idx % BLOCK_SIZE;
    }

    void ensureSlotExists(uint32_t idx) {
        const std::size_t neededBlocks = static_cast<std::size_t>(blockIndex(idx)) + 1;

        while (blocks_.size() < neededBlocks) {
            blocks_.emplace_back(std::make_unique<Slot[]>(BLOCK_SIZE));
        }
    }

    Slot& slotAt(uint32_t idx) {
        assert(idx < slotCount_);
        return blocks_[blockIndex(idx)][slotIndexInBlock(idx)];
    }

    const Slot& slotAt(uint32_t idx) const {
        assert(idx < slotCount_);
        return blocks_[blockIndex(idx)][slotIndexInBlock(idx)];
    }

    
public:
    FreeList() = default;
    ~FreeList() {
        clear();
    }

    FreeList(const FreeList&) = delete;
    FreeList& operator=(const FreeList&) = delete;

    FreeList(FreeList&&) noexcept = default;
    FreeList& operator=(FreeList&&) noexcept = default;

    std::size_t size() const noexcept {
        return liveCount_;
    }

    std::size_t slot_count() const noexcept {
        return slotCount_;
    }

    bool empty() const noexcept {
        return liveCount_ == 0;
    }

    template<typename... Args>
    uint32_t emplace(Args&&... args) {
        uint32_t idx = kNone;

        if (freeHead_ != kNone) {
            idx = freeHead_;
            Slot& s = slotAt(idx);

            freeHead_ = s.next;

            assert(!s.alive);
            std::construct_at(s.value(), std::forward<Args>(args)...);
            s.alive = true;

            ++liveCount_;
            return idx;
        }

        idx = slotCount_;

        Slot& s = slotAt(idx);
        assert(!s.alive);

        std::construct_at(s.value(), std::forward<Args>(args)...);
        s.alive = true;
        s.next  = kNone;

        ++slotCount_;
        ++liveCount_;

        return idx;
    }

    uint32_t allocate(const T& value) {
        return emplace(value);
    }

    uint32_t allocate(T&& value) {
        return emplace(std::move(value));
    }

    void free(uint32_t idx) {
        Slot& s = slotAt(idx);

        std::destroy_at(s.value());
        s.alive = false;

        ++s.generation;
        s.next = freeHead_;
        freeHead_ = idx;

        --liveCount_;
    }

    void clear() {
        if (slotCount_ == 0) {
            freeHead_ = kNone;
            liveCount_ = 0;
            return;
        }

        freeHead_ = kNone;

        for (uint32_t i = 0; i < slotCount_; ++i) {
            Slot& s = slotAt(i);

            if (s.alive) {
                std::destroy_at(s.value());
                s.alive = false;
            }

            s.next = freeHead_;
            freeHead_ = i;
        }

        liveCount_ = 0;
    }

    template<typename Fn>
    void for_each(Fn&& fn) {
        for (uint32_t i = 0; i < slotCount_; ++i) {
            Slot& s = slotAt(i);
            if (s.alive) {
                fn(i, *s.value());
            }
        }
    }

    template<typename Fn>
    void for_each(Fn&& fn) const {
        for (uint32_t i = 0; i < slotCount_; ++i) {
            const Slot& s = slotAt(i);
            if (s.alive) {
                fn(i, *s.value());
            }
        }
    }
};