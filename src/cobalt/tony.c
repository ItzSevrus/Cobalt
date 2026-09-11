/*
    Author: Sahil Gour
    Licensed Under: MIT LICENSE
*/
#include "../../include/cobalt/tony.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    tPtr* pointer;
} MemEntry;

static MemEntry* MEM_POOL = NULL;
static size_t MEM_CAP = 0; /* Number of MemEntry slots allocated */
static size_t MEM_COUNT = 0; /* Number of occupied MemEntry slots */

static bool init_pool(void)
{
    /*
    This function initializes the MEM_POOL.
    
    malloc(0):-
        We intentionally request zero bytes to obtain an initial
        allocation state for the pool.

        The implementation may return NULL or a non-NULL pointer.
        A non-NULL result can later be passed to realloc() or free().
    */

    if (MEM_POOL != NULL){
        printf("[X] Error! Pool already initialized!\n");
        return false;
    }

    MemEntry* pointer = malloc(0);
    if (pointer == NULL){
        printf("[X] Error! Failed to initialize memory pool!\nfunc::init_pool()\n");
        perror("malloc");
        return false;
    }

    MEM_POOL = pointer;

    return true;
}

static bool incr_pool(void)
{
    /*
    This function increases the memory pool if its not NULL.
    We are multiplying memory size by 2 for newSize to prevent
    multiple incr_pool() calls because it will be expensive
    */
    if (MEM_POOL == NULL){
        printf("[X] Error! Can't increase memory pool, pool is NULL\nfunc::incr_pool()\n");
        return false;
    }

    size_t newSize;
    
    if (MEM_CAP == 0){
        newSize = 2;
    } else {
        newSize = MEM_CAP * 2;
    }

    MemEntry* newPool = realloc(MEM_POOL, newSize * sizeof(MemEntry));
    if (newPool == NULL){
        printf("[X] Error! Can't increase pool, failed to allocate memory!\nfunc::incr_pool()\n");
        perror("realloc");
        return false;
    }

    MEM_POOL = newPool;
    MEM_CAP = newSize;
    return true;
}

static int get_empty_slot(void)
{
    int index = -1;

    for (size_t i=0; i<MEM_CAP; i++){
        if (MEM_POOL[i].pointer == NULL){
            index = i;
            break;
        }
    }
    return index;
}

static bool add_to_pool(tPtr *pointer)
{
    /*
    This function takes a tPtr's pointer and adds it
    to the memory pool.
    */

    if (MEM_POOL == NULL) {
        printf("[X] Error! Memory pool is NULL\n");
        printf("func::add_to_pool()\n");
        return false;
    }

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
    pointer->id = loc;
    MEM_COUNT++;

    return true;
}

tPtr talloc(size_t size)
{
    tPtr result = {0};

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

    tPtr *pointer = malloc(sizeof(tPtr));

    if (pointer == NULL) {
        printf("[X] Error! Unable to allocate tPtr!\n");
        printf("func::talloc()\n");
        perror("malloc");

        free(data);
        return result;
    }

    pointer->ptr = data;

    if (!add_to_pool(pointer)) {
        printf("[X] Error! Unable to register pointer in memory pool!\n");
        printf("func::talloc()\n");

        free(pointer);
        free(data);

        return result;
    }

    result = *pointer;

    return result;
}

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

    tPtr *pointer = MEM_POOL[loc].pointer;

    if (pointer == NULL) {
        printf("[X] Pointer already destroyed or slot is empty!\n");
        printf("func::destroy()\n");
        return false;
    }

    if (pointer->ptr != tptr.ptr) {
        printf("[X] tPtr does not match pool entry!\n");
        printf("func::destroy()\n");
        return false;
    }

    free(pointer->ptr);
    free(pointer);

    MEM_POOL[loc].pointer = NULL;
    MEM_COUNT--;

    return true;
}

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

    tPtr *poolPtr = MEM_POOL[loc].pointer;

    if (poolPtr == NULL) {
        printf("[X] Pointer does not exist in memory pool!\n");
        printf("func::trealloc()\n");
        return result;
    }

    if (poolPtr->ptr != pointer.ptr) {
        printf("[X] tPtr does not match pool entry!\n");
        printf("func::trealloc()\n");
        return result;
    }

    void *newPtr = realloc(poolPtr->ptr, newSize);

    if (newPtr == NULL && newSize != 0) {
        printf("[X] Failed to reallocate memory!\n");
        printf("func::trealloc()\n");
        perror("realloc");
        return result;
    }

    poolPtr->ptr = newPtr;
    pointer.ptr = newPtr;

    result = pointer;

    return result;
}

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

    free(MEM_POOL);

    MEM_POOL = NULL;
    MEM_CAP = 0;
    MEM_COUNT = 0;

    return true;
}

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