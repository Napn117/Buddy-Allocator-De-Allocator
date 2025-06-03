//////////////////////////////////////////////////////////////////////////////////
//
//   Program Name:  Buddy System Algorithm
//
//   Description:  Buddy System Algorithm
//
//   Submitted by: Jesse Easton-Harris 12013205, Luke Roycroft 22013719
//
//////////////////////////////////////////////////////////////////////////////////

#include "buddysys.h"
#include <cmath>

// Define a constant variable for the maximum of 2^K (SET TO 9 FOR RUNNING SIMPLE TEST)
const int maxK = 25;
// Define a constant variable for the minimum of 2^K. We choose 6 for complete_test because 2^6 = 64. This works because our struct is 32 bytes. (SET TO 1 FOR RUNNING SIMPLE TEST)
const int minK = 6;
// Define the amount of bins needed inside of the freeList Array. Using 25 - 6 + 1 equals 20. We add 1 to ensure the last bin is included.
const int freeListSize = maxK - minK + 1;

// Create the array of pointers to Node for the freeList bins. Each bin is made from 2^6 through to 2^25. We initialize to nullptr to ensure all bins are empty to start.
Node *freeList[freeListSize] = {nullptr};

//////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                              //
// Helper functions                                                                             //
//                                                                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////

void createFreeList()
{
    int index = maxK - minK;
    wholememory->alloc = 0;        // Mark the block as unallocated
    freeList[index] = wholememory; // Store block in free list at last index
}

void add_to_freelist(Node *unallocated)
{
    int index = log2(unallocated->size + sizeof(Node)) - minK; // Index number of the bin we want to add block to
    unallocated->next = unallocated->previous = nullptr;       // Remove any connections to other blocks
    unallocated->alloc = 0;                                    // Set the block as unallocated

    if (freeList[index] == nullptr) // Add to head of the list at index
    {
        freeList[index] = unallocated;
    }
    else // Add to the end of the list at index
    {
        Node *temp = freeList[index];
        while (temp->next != nullptr) // Get to the end of the list
        {
            temp = temp->next;
        }
        temp->next = unallocated;     // Point to the new block
        unallocated->previous = temp; // Point back at the end of list
        unallocated->next = nullptr;
    }
}

void remove_from_freelist(Node *allocated)
{
    int index = log2(allocated->size + sizeof(Node)) - minK; // Index number of the bin we want to remove block from

    if (freeList[index] == allocated && freeList[index]->next == nullptr) // Allocated block is at the head of bin and is the only block at index
    {
        freeList[index] = nullptr;
        allocated->next = nullptr;
        allocated->previous = nullptr;
    }
    else if (freeList[index] == allocated && freeList[index]->next != nullptr) // Allocated block is at the head of bin and the list has more blocks
    {
        freeList[index] = freeList[index]->next;
        freeList[index]->previous = nullptr;
        allocated->next = nullptr;
        allocated->previous = nullptr;
    }
    else // Allocated block is down the list somewhere
    {
        Node *temp = freeList[index];
        Node *previous = nullptr;
        while (temp && temp != allocated) // Find the allocated block in the list at index
        {
            previous = temp;
            temp = temp->next;
        }
        if (temp->next == nullptr) // Allocated block is last in list
        {
            previous->next = nullptr;
        }
        else // Allocated block is between two blocks
        {
            previous->next = temp->next;
            temp->next->previous = previous;
        }
        allocated->next = nullptr; // remove connections to other blocks
        allocated->previous = nullptr;
    }
}

Node *get_from_freelist(int index)
{
    Node *block = nullptr;
    block = freeList[index];     // Get the first block in bin at index
    remove_from_freelist(block); // Remove it from the free list
    return block;
}

void split_block(int index)
{
    int exponent = index + minK;          // Exponent of the full block to split
    long long int size = 1LL << exponent; // Shift 1 left by the exponent. Same as 2^exponent.
    long long int halfSize = size >> 1;   // Size of new half blocks. Shifting right by 1 halfs the size.

    Node *block_a = get_from_freelist(index);             // Get the full block to split
    Node *block_b = (Node *)((byte *)block_a + halfSize); // New smaller block start at half size away from the start of the full block
    block_a->size = halfSize - sizeof(Node);              // Set data size
    block_b->size = halfSize - sizeof(Node);

    add_to_freelist(block_a); // Add both blocks to the free list
    block_a->alloc = 0;       // Set them to unallocated
    add_to_freelist(block_b);
    block_b->alloc = 0;
}
Node *get_buddy(Node *buddy)
{
    uintptr_t base = (uintptr_t)wholememory;
    uintptr_t offset = (uintptr_t)buddy - base;
    uintptr_t size = buddy->size + sizeof(Node);

    return (Node *)(base + (offset ^ size)); // From the start of the wholememory, the buddy of the block will be at the XOR(flipped bits) of the offset and the size of the block
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                              //
// Main functions                                                                               //
//                                                                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////

void *buddyMalloc(int request_memory)
{
    int memory_request = request_memory + sizeof(Node); // Need to add the Node size to the request memory to ensure we can store the whole request

    int k = max(minK, (int)ceil(log2f(memory_request))); // The exponent k is the largest of our smallest allowable k and the rounded up log of the memory request

    int index = k - minK; // Index of block needed for request

    if (k > maxK) // Request is larger than wholememory
    {
        return nullptr;
    };

    Node *allocating_block = nullptr;

    if (freeList[index] != nullptr) // Check the free list at the index to see if there is a free block there
    {
        allocating_block = get_from_freelist(index);    // Grab that block
        allocating_block->alloc = 1;                    // Mark it as allocated
        return (byte *)allocating_block + sizeof(Node); // Return the data part of the block
    }
    else // No appropriate block so we need to create one
    {

        int finder = index; // Starting from our ideal index

        while (finder < freeListSize && freeList[finder] == nullptr) // Look through each index larger than the one we want until we find a free block
        {
            finder++;
        }
        while (finder > index) // When we find a block, we need to split it down to the index size that we want.
        {
            split_block(finder);
            --finder;
        }
        allocating_block = get_from_freelist(index);    // Now there is a block at the index we want we can grab it
        allocating_block->alloc = 1;                    // And set it to allocated
        return (byte *)allocating_block + sizeof(Node); // Return the data part of the block
    }
}

int buddyFree(void *p)
{
    Node *dealloc = (Node *)((byte *)p - sizeof(Node)); // Get the full address of the Node to be freed
    dealloc->alloc = 0;                                 // Set it as unallocated

    while (true) // Until no suitable buddy is found to merge with
    {
        Node *buddy = get_buddy(dealloc);                      // Get the buddy of the current block to deallocate
        if (buddy->alloc == 1 || buddy->size != dealloc->size) // If the buddy is allocated or the blocks arent the same size for coalescing, leave the loop and add block to free list
        {
            break;
        }
        remove_from_freelist(buddy); // The buddy is not allocated and its the same size. Free the buddy from the list ready to merge
        if (buddy < dealloc)         // The new merged block must take the lower address value of the buddies. If the buddy is the lower address it becomes the "main block"
        {
            dealloc = buddy;
        }
        dealloc->size = dealloc->size * 2 + sizeof(Node); // Set the new size of the block
    }
    add_to_freelist(dealloc); // Add the block to the free list
    return 1;
}
