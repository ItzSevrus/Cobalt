/*
    Author: Sahil Gour
    Licensed Under: MIT LICENSE
*/
#include "../../include/cobalt/tony.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/*
    A single entry in Tony's memory pool.

    `pointer` points to a dynamically allocated tPtr.
    The tPtr stores the actual allocated memory address
    and the pool slot ID associated with that allocation.
*/
typedef struct {
    tPtr* pointer;
} MemEntry;

/*
    Tony's global memory pool.

    MEM_POOL:
        Dynamically allocated array of MemEntry objects.

    MEM_CAP:
        Number of MemEntry slots currently allocated.

    MEM_COUNT:
        Number of currently occupied slots.
*/
static MemEntry* MEM_POOL = NULL;
static size_t MEM_CAP = 0;
static size_t MEM_COUNT = 0;


/*
    Initializes Tony's memory pool.

    The pool starts with zero capacity. A zero-byte allocation
    is used to establish the initial allocation state, after
    which incr_pool() expands the pool when the first entry
    is added.
*/
static bool init_pool(void)
{
    if (MEM_POOL != NULL) {
        printf("[X] Error! Pool already initialized!\n");
        return false;
    }

    /*
        malloc(0) is permitted to return either NULL or a
        unique pointer that may later be passed to realloc()
        or free().
    */
    MemEntry* pointer = malloc(0);

    if (pointer == NULL) {
        printf("[X] Error! Failed to initialize memory pool!\n");
        printf("func::init_pool()\n");
        perror("malloc");
        return false;
    }

    MEM_POOL = pointer;

    return true;
}


/*
    Doubles the capacity of the memory pool.

    When the pool has zero capacity, it is initialized with
    two slots. Otherwise, the current capacity is doubled.

    Growing exponentially reduces the number of reallocations
    required as more allocations are registered.
*/
static bool incr_pool(void)
{
    if (MEM_POOL == NULL) {
        printf("[X] Error! Can't increase memory pool, pool is NULL\n");
        printf("func::incr_pool()\n");
        return false;
    }

    size_t newSize;

    if (MEM_CAP == 0) {
        newSize = 2;
    } else {
        newSize = MEM_CAP * 2;
    }

    MemEntry* newPool = realloc(MEM_POOL, newSize * sizeof(MemEntry));

    if (newPool == NULL) {
        printf("[X] Error! Can't increase pool, failed to allocate memory!\n");
        printf("func::incr_pool()\n");
        perror("realloc");
        return false;
    }

    MEM_POOL = newPool;
    MEM_CAP = newSize;

    return true;
}


/*
    Finds the first unoccupied slot in the memory pool.

    Returns:
        The index of the first empty slot.
        -1 if no empty slot exists.
*/
static int get_empty_slot(void)
{
    int index = -1;

    for (size_t i = 0; i < MEM_CAP; i++) {
        if (MEM_POOL[i].pointer == NULL) {
            index = i;
            break;
        }
    }

    return index;
}


/*
    Registers a tPtr in Tony's memory pool.

    The tPtr's ID is assigned to the index of its pool entry.
    This allows Tony to locate the corresponding allocation
    directly using:

        MEM_POOL[tptr.id]

    without scanning the entire pool.
*/
static bool add_to_pool(tPtr *pointer)
{
    if (MEM_POOL == NULL) {
        printf("[X] Error! Memory pool is NULL\n");
        printf("func::add_to_pool()\n");
        return false;
    }

    /*
        Grow the pool when all currently allocated slots
        are occupied.
    */
    if (MEM_CAP == MEM_COUNT) {
        if (!incr_pool()) {
            return false;
        }
    }

    int loc = get_empty_slot();

    if (loc == -1) {
        printf("[X] Error! Unable to find an empty pool slot!\n");
        printf("func::add_to_pool()\n");
        return false;
    }

    MemEntry entry;
    entry.pointer = pointer;

    MEM_POOL[loc] = entry;

    /*
        The ID is the pool index. This provides O(1) lookup
        for operations such as destroy() and trealloc().
    */
    pointer->id = loc;

    MEM_COUNT++;

    return true;
}


/*
    Allocates memory and registers it with Tony.

    Returns:
        A tPtr containing the allocated memory and its
        Tony pool ID.

    On failure, a zero-initialized tPtr is returned.
*/
tPtr talloc(size_t size)
{
    tPtr result = {0};

    /*
        Lazily initialize Tony on the first allocation.
    */
    if (MEM_POOL == NULL) {
        if (!init_pool()) {
            printf("[X] Error! Unable to allocate memory, pool isn't initialized!\n");
            printf("func::talloc()\n");
            return result;
        }
    }

    void *data = malloc(size);

    if (data == NULL) {
        printf("[X] Error! Unable to allocate memory!\n");
        printf("func::talloc()\n");
        perror("malloc");
        return result;
    }

    /*
        The tPtr itself is stored dynamically because the
        pool must retain it after talloc() returns its copy
        to the caller.
    */
    tPtr *pointer = malloc(sizeof(tPtr));

    if (pointer == NULL) {
        printf("[X] Error! Unable to allocate tPtr!\n");
        printf("func::talloc()\n");
        perror("malloc");

        free(data);
        return result;
    }

    pointer->ptr = data;

    /*
        Register the allocation before returning it.
        If registration fails, both allocations must be
        released to prevent a memory leak.
    */
    if (!add_to_pool(pointer)) {
        printf("[X] Error! Unable to register pointer in memory pool!\n");
        printf("func::talloc()\n");

        free(pointer);
        free(data);

        return result;
    }

    /*
        Return a copy of the pool-owned tPtr.
        Both tPtrs reference the same allocated data.
    */
    result = *pointer;

    return result;
}


/*
    Destroys a single Tony allocation.

    The tPtr ID is used for direct O(1) pool lookup.
    The stored data pointer is also compared against the
    supplied tPtr to prevent an old or invalid tPtr from
    destroying a different allocation occupying the same slot.
*/
bool destroy(tPtr tptr)
{
    if (MEM_POOL == NULL) {
        printf("[X] Can't destroy, memory pool is NULL!\n");
        printf("func::destroy()\n");
        return false;
    }

    size_t loc = tptr.id;

    if (loc >= MEM_CAP) {
        printf("[X] Invalid tPtr ID!\n");
        printf("func::destroy()\n");
        return false;
    }

    /*
        Directly access the pool entry using the tPtr ID.
        No pool traversal is required.
    */
    tPtr *pointer = MEM_POOL[loc].pointer;

    if (pointer == NULL) {
        printf("[X] Pointer already destroyed or slot is empty!\n");
        printf("func::destroy()\n");
        return false;
    }

    /*
        Verify that the supplied tPtr still refers to the
        allocation currently occupying this pool slot.
    */
    if (pointer->ptr != tptr.ptr) {
        printf("[X] tPtr does not match pool entry!\n");
        printf("func::destroy()\n");
        return false;
    }

    free(pointer->ptr);
    free(pointer);

    /*
        The slot remains available for future allocations.
        Tony does not shrink the pool here.
    */
    MEM_POOL[loc].pointer = NULL;
    MEM_COUNT--;

    return true;
}


/*
    Resizes an existing Tony allocation using realloc().

    Returns:
        An updated tPtr containing the new memory address.

    The allocation may be moved by realloc(), so both the
    pool-owned tPtr and the returned tPtr must receive the
    new pointer.
*/
tPtr trealloc(tPtr pointer, size_t newSize)
{
    tPtr result = {0};

    if (MEM_POOL == NULL) {
        printf("[X] Can't realloc, memory pool is NULL!\n");
        printf("func::trealloc()\n");
        return result;
    }

    size_t loc = pointer.id;

    if (loc >= MEM_CAP) {
        printf("[X] Invalid tPtr ID!\n");
        printf("func::trealloc()\n");
        return result;
    }

    /*
        Use the ID for direct O(1) lookup.
    */
    tPtr *poolPtr = MEM_POOL[loc].pointer;

    if (poolPtr == NULL) {
        printf("[X] Pointer does not exist in memory pool!\n");
        printf("func::trealloc()\n");
        return result;
    }

    /*
        Ensure the supplied tPtr matches the allocation
        currently stored at this pool slot.
    */
    if (poolPtr->ptr != pointer.ptr) {
        printf("[X] tPtr does not match pool entry!\n");
        printf("func::trealloc()\n");
        return result;
    }

    void *newPtr = realloc(poolPtr->ptr, newSize);

    /*
        realloc() returns NULL on failure while leaving the
        original allocation untouched. A zero-size request
        is handled separately because realloc(ptr, 0) may
        free the allocation and return NULL.
    */
    if (newPtr == NULL && newSize != 0) {
        printf("[X] Failed to reallocate memory!\n");
        printf("func::trealloc()\n");
        perror("realloc");
        return result;
    }

    /*
        Update both copies because realloc() may have moved
        the allocation to a new memory address.
    */
    poolPtr->ptr = newPtr;
    pointer.ptr = newPtr;

    result = pointer;

    return result;
}


/*
    Frees every allocation currently registered with Tony
    and destroys the memory pool itself.

    After successful completion, Tony returns to its initial
    uninitialized state.
*/
bool freeAll(void)
{
    if (MEM_POOL == NULL) {
        printf("[X] Can't free a null memory pool!\n");
        printf("func::freeAll()\n");
        return false;
    }

    for (size_t i = 0; i < MEM_CAP; i++) {
        tPtr *pointer = MEM_POOL[i].pointer;

        if (pointer != NULL) {
            free(pointer->ptr);
            free(pointer);

            MEM_POOL[i].pointer = NULL;
        }
    }

    /*
        Destroy the pool itself after all registered
        allocations have been released.
    */
    free(MEM_POOL);

    MEM_POOL = NULL;
    MEM_CAP = 0;
    MEM_COUNT = 0;

    return true;
}


/*
    Prints Tony's current memory pool state.

    Displays:-
        CAP  - total number of allocated pool slots.
        USED - number of occupied pool slots.
        ID   - Tony allocation ID / pool index.
        PTR  - actual allocated memory address.

    Empty pool slots are displayed as '-'.
*/
void debug_mem_pool(void)
{
    printf("\nTony Memory Pool\n");
    printf("----------------\n");
    printf("CAP: %zu | USED: %zu\n\n", MEM_CAP, MEM_COUNT);

    printf(" ID   PTR\n");
    printf("------------\n");

    for (size_t i = 0; i < MEM_CAP; i++) {
        tPtr *pointer = MEM_POOL[i].pointer;

        if (pointer == NULL) {
            printf(" %2zu   -\n", i);
        } else {
            printf(" %2zu   %p\n", pointer->id, pointer->ptr);
        }
    }

    printf("\n");
}