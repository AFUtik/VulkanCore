#pragma once

#include <unordered_map>
#include <array>
#include <cstdint>
#include <chrono>
#include <string>

namespace myvk
{

enum class MemoryType
{
    CPU,
    GPU,
    CPU_GPU
};

struct AllocationInfo
{
    std::array<char, 32> name{"Undefined"};
    size_t size = 0;
    MemoryType type = MemoryType::CPU;
    std::chrono::steady_clock::time_point created;
};

class MemoryTracker
{
public:
    void track(const void* ptr, const AllocationInfo& info);
    void untrack(const void* ptr);
    
    void reset();
public:
    std::unordered_map<const void*, AllocationInfo> allocations;

    size_t cpuMemory = 0;
    size_t gpuMemory = 0;

    size_t peakCPU = 0;
    size_t peakGPU = 0;

    size_t totalAllocated = 0;
    size_t totalFreed = 0;
};

}