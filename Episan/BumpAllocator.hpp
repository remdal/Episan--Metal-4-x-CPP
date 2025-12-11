#ifndef BUMPALLOCATOR_HPP
#define BUMPALLOCATOR_HPP

#include <Metal/Metal.hpp>
#include <cassert>
#include <cstdint>
#include <tuple>

namespace mem
{
constexpr uint64_t alignUp(uint64_t n, uint64_t alignment)
{
    return (n + alignment - 1) & ~(alignment - 1);
}
}

/// This allocator isn't thread-safe. For multithreading encoding,
/// create one of these instances per thread per frame.
class BumpAllocator
{
public:
    BumpAllocator(MTL::Device* pDevice, size_t capacityInBytes, MTL::ResourceOptions resourceOptions);
    ~BumpAllocator();

    void reset() { _offset = 0; }

    template <typename T>
    std::pair<T*, uint64_t> allocate(uint64_t count = 1) noexcept
    {
        // Shader converter requires an alignment of 8-bytes:
        uint64_t allocSize = mem::alignUp(sizeof(T) * count, 8);
        
        // If you hit this assert, the allocation data doesn't fit in
        // the amount estimated.
        assert((_offset + allocSize) <= _capacity);

        T* dataPtr          = reinterpret_cast<T*>(_contents + _offset);
        uint64_t dataOffset = _offset;

        _offset += allocSize;

        return { dataPtr, dataOffset };
    }

    MTL::Buffer* baseBuffer() const noexcept
    {
        return _pBuffer;
    }

private:
    MTL::Buffer* _pBuffer;
    uint64_t _offset;
    uint64_t _capacity;
    uint8_t* _contents;
};

#endif // BUMPALLOCATOR_HPP
