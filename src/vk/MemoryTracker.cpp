#include "vk/MemoryTracker.hpp"

namespace myvk
{ 
 
    void MemoryTracker::track(const void* ptr, const AllocationInfo& info)
    {
        allocations[ptr] = info;

        totalAllocated += info.size;

        if (info.type == MemoryType::GPU)
            gpuMemory += info.size;
        else
            cpuMemory += info.size;

        peakCPU = std::max(peakCPU, cpuMemory);
        peakGPU = std::max(peakGPU, gpuMemory);
    }

    void MemoryTracker::untrack(const void* ptr)
    {
        auto it = allocations.find(ptr);
        if (it == allocations.end()) return;

        const auto& info = it->second;

        totalFreed += info.size;

        if (info.type == MemoryType::GPU)
            gpuMemory -= info.size;
        else
            cpuMemory -= info.size;

        allocations.erase(it);
    }

    void MemoryTracker::reset()
    {
        allocations.clear();
        cpuMemory = gpuMemory = 0;
        totalAllocated = totalFreed = 0;
    }
    
}