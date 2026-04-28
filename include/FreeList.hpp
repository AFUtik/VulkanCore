#pragma once

#include <climits>
#include <memory>
#include <cstdint>
#include <cassert>
#include <vector>

template<typename T>
class FreeList {
public:
    FreeList(size_t initial_capacity = 16) {
        s_ptrs.reserve(initial_capacity);
        next.reserve(initial_capacity);
    }

    inline uint32_t create(T sptr) {
        uint32_t idx = obtain_free_index();
        s_ptrs[idx] = std::move(sptr);
        return idx;
    }

    inline void remove(uint32_t handle) {
        s_ptrs[handle] = T();
        next[handle] = free_head;
        free_head = handle;
    }

    inline size_t size() {
        return s_ptrs.size();
    }

    inline T& operator[](uint32_t handle) {
        return s_ptrs[handle];
    }
private:
    inline uint32_t obtain_free_index() { 
        if (free_head != invalid_index) { 
            uint32_t idx = free_head; 
            free_head = next[idx]; 
            return idx; 
        } 
        next.emplace_back(); 
        s_ptrs.emplace_back();
        return static_cast<uint32_t>(next.size() - 1); 
    }

    static constexpr uint32_t invalid_index = std::numeric_limits<uint32_t>::max();

    std::vector<T> s_ptrs;
    std::vector<uint32_t> next;

    int free_head = invalid_index;
};