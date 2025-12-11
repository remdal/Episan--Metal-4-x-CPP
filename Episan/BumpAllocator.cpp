//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Copyright 2024 Apple Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "BumpAllocator.hpp"

BumpAllocator::BumpAllocator(MTL::Device* pDevice, size_t capacityInBytes, MTL::ResourceOptions resourceOptions)
{
    assert(resourceOptions != MTL::ResourceStorageModePrivate);
    _offset   = 0;
    _capacity = capacityInBytes;
    _pBuffer  = pDevice->newBuffer(capacityInBytes, resourceOptions);
    _contents = (uint8_t*)_pBuffer->contents();
}
BumpAllocator::~BumpAllocator()
{
    _pBuffer->release();
}


class DeepseekBumpAllocator
{
    MTL::Heap* heap;
    size_t offset = 0;
    size_t capacity;
    
public:
    DeepseekBumpAllocator(MTL::Device* device, size_t size)
    {
        MTL::HeapDescriptor* desc = MTL::HeapDescriptor::alloc()->init();
        desc->setSize(size);
        heap = device->newHeap(desc);
        capacity = size;
        desc->release();
    }
    
    MTL::Buffer* allocateBuffer(size_t size, MTL::ResourceOptions options)
    {
        if (offset + size > capacity) return nullptr;
        MTL::Buffer* buffer = heap->newBuffer(size, options, offset);
        offset += mem::alignUp(size, 256);
        return buffer;
    }
    
    void reset() { offset = 0; }
};
