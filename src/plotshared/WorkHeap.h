#pragma once
#include "threading/AutoResetSignal.h"
#include "SPCQueue.h"
#include "util/Array.h"

// A simple heap to be used as our working buffer provider
// for doing plotting work in memory and I/O operations.
// It is meant to have a very small amount of allocations, therefore
// allocations are tracked in a small table that is searched linerarly.

class WorkHeap
{
private:
    // Represents a portion of unallocated space in our heap/work buffer
    struct HeapEntry
    {
        byte*  address;
        size_t size;
    };

public:
    WorkHeap( size_t size, byte* heapBuffer );
    ~WorkHeap();

    // Allocate a buffer on the heap.
    // If no space is available it will block until
    // some space has become available again.
    byte* Alloc( size_t size );

    // Add a to the pending release list.
    // The buffer won't actually be released until an allocation
    // attempt is called or an explicit call AddPendingReleases() to is made.
    // This is meant to be called by a producer thread.
    void  Release( byte* buffer );

    // Makes pending released allocations available to the heap for allocation again.
    void AddPendingReleases();

private:

private:
    byte*                _heap;
    size_t               _heapSize;             // Size of our work heap
    size_t               _usedHeapSize;         // How much heap space is currently being used

    Array<HeapEntry>     _heapTable;            // Tracks unallocated space in our work heap
    Array<HeapEntry>     _allocationTable;      // Tracks sizes and for currently allocated buffers. Used when performing releases.

    SPCQueue<byte*, 256> _pendingReleases;      // Released buffers waiting to be re-added to the heap table
    AutoResetSignal      _releaseSignal;        // Used to signal that there's pending released buffers
};


