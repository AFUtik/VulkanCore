#pragma once

#include <array>
#include <vector>
#include <limits>
#include <memory>

using u64 = std::size_t;

template<
    typename T,
    typename I,
    u64 Capacity    
>
class AllocatedFreelist {
public:
    static constexpr I invalid = std::numeric_limits<I>::max();
    
    AllocatedFreelist() {}

    inline I push(T&& object) {
        I index = obtain_free_index();
        if(index>=_size) _size++;
        assert(_size <= Capacity);

        array[index] = std::forward<T>(object);
        return index;
    }

    inline I push(T& object) {
        I index = obtain_free_index();
        if(index>=_size) _size++;
        assert(_size <= Capacity);

        array[index] = std::move(object);
        return index;
    }

    inline void erase(u64 index) {
        if(index == _size - 1) _size--;

        array[index] = T();
        next[index] = free_head;
        free_head = index;
    }

    inline T& operator[](u64 index) {
        assert(index < _size);
        return array[index];
    }

    inline const T& operator[](u64 index) const {
        assert(index < _size);
        return array[index];
    }

    inline u64 size() {return _size;} 
    
    inline u64 obtain_free_index() {
        assert(free_head!=Capacity);
        if(free_head!=invalid) {
            u64 idx = free_head;
            free_head = next[idx]; 
            return idx; 
        }
        return _size;
    }
private:
    std::array<T, Capacity> array;
    std::array<I, Capacity> next;
    u64 free_head = invalid;
    u64 _size = 0;
};

template<typename T, typename I = uint32_t>
class Freelist {
public:
    static constexpr I invalid = std::numeric_limits<I>::max();

    Freelist(I capacity = 256)
        : capacity(capacity)
    {
        data = std::unique_ptr<T[]>(new T[capacity]);
        next = std::unique_ptr<I[]>(new I[capacity]);

        for (I i = 0; i < capacity; i++) next[i] = invalid;
    }

    Freelist(const Freelist&) = delete;
    Freelist& operator=(const Freelist&) = delete;

    I push(T&& object) {
        I index = obtain_free_index();
        ensure_capacity_if_needed(index);

        data[index] = std::move(object);

        if (index == size_) {
            size_++;
        }

        return index;
    }

    I push(const T& object) {
        I index = obtain_free_index();
        ensure_capacity_if_needed(index);

        data[index] = object;

        if (index == size_) {
            size_++;
        }

        return index;
    }

    void erase(I index) {
        assert(index < size_);

        data[index] = T{};
        next[index] = free_head;
        free_head = index;
    }

    T& operator[](I index) {
        return data[index];
    }

    const T& operator[](I index) const {
        return data[index];
    }

    I size() const {
        return size_;
    }

private:
    std::unique_ptr<T[]> data;
    std::unique_ptr<I[]> next;

    I capacity = 0;
    I size_ = 0;

    I free_head = invalid;

    I obtain_free_index() {
        if (free_head != invalid) {
            I idx = free_head;
            free_head = next[idx];
            return idx;
        }
        return size_;
    }

    void ensure_capacity_if_needed(I index) {
        if (index < capacity) return;

        grow(capacity * 2);
    }

    void grow(I newCapacity) {
        std::unique_ptr<T[]> newData(new T[newCapacity]);
        std::unique_ptr<I[]> newNext(new I[newCapacity]);

        for (I i = 0; i < capacity; i++) {
            newData[i] = std::move(data[i]);
            newNext[i] = next[i];
        }

        for (I i = capacity; i < newCapacity; i++) {
            newNext[i] = invalid;
        }

        data = std::move(newData);
        next = std::move(newNext);

        capacity = newCapacity;
    }
};