/*
    Author: Sahil Gour
    Licensed Under: MIT LICENSE
*/

#include "../../include/cobalt/Clist.h"
#include "../../include/cobalt/dtypes.h"
#include <stdbool.h>

bool __listAppend(Clist *list, void *data, DataType type)
{
    if (list == NULL) {
        return false;
    }

    tPtr pointer = talloc(sizeof(tPtr));

    if (pointer.ptr == NULL) {
        return false;
    }

    tPtr *newPtr = pointer.ptr;

    newPtr->ptr = data;
    newPtr->type = type;

    if (list->items.ptr == NULL) {
        list->items = talloc(sizeof(tPtr *));
    } else {
        tPtr resized = trealloc(
            list->items,
            (list->length + 1) * sizeof(tPtr *)
        );

        if (resized.ptr == NULL) {
            return false;
        }

        list->items = resized;
    }

    ((tPtr **)list->items.ptr)[list->length] = newPtr;

    list->length++;

    return true;
}

tPtr listGet(Clist *list, size_t index)
{
    tPtr result = {0};

    if (list == NULL) {
        return result;
    }

    if (index >= list->length) {
        return result;
    }

    tPtr **items = list->items.ptr;

    if (items[index] == NULL) {
        return result;
    }

    return *items[index];
}

bool listRemove(Clist *list, size_t index)
{
    if (list == NULL || index >= list->length) {
        return false;
    }

    tPtr **items = list->items.ptr;

    for (size_t i = index; i < list->length - 1; i++) {
        items[i] = items[i + 1];
    }

    list->length--;

    return true;
}

tPtr listPop(Clist *list)
{
    tPtr result = {0};

    if (list == NULL || list->length == 0) {
        return result;
    }

    tPtr **items = list->items.ptr;

    tPtr *pointer = items[list->length - 1];

    if (pointer == NULL) {
        return result;
    }

    result = *pointer;

    list->length--;

    return result;
}

bool listSet(Clist *list, size_t index, tPtr *pointer)
{
    if (list == NULL || pointer == NULL) {
        return false;
    }

    if (index >= list->length) {
        return false;
    }

    tPtr **items = list->items.ptr;

    items[index] = pointer;

    return true;
}

bool listClear(Clist *list)
{
    if (list == NULL) {
        return false;
    }

    list->length = 0;

    return true;
}

bool listSwap(Clist *list, size_t index_to_swap, size_t index_value_toswap)
{
    if (list == NULL) {
        return false;
    }

    if (index_to_swap >= list->length ||
        index_value_toswap >= list->length) {
        return false;
    }

    tPtr **items = list->items.ptr;

    tPtr *temp = items[index_to_swap];
    items[index_to_swap] = items[index_value_toswap];
    items[index_value_toswap] = temp;

    return true;
}

void listReverse(Clist *list)
{
    if (list == NULL || list->length < 2) {
        return;
    }

    size_t left = 0;
    size_t right = list->length - 1;

    while (left < right) {
        listSwap(list, left, right);

        left++;
        right--;
    }
}

static void __listPrint(Clist *list)
{
    if (list == NULL || list->length == 0) {
        printf("[]");
        return;
    }

    printf("[");

    tPtr **items = list->items.ptr;

    for (size_t i = 0; i < list->length; i++) {
        tPtr pointer = *items[i];

        if (pointer.type == TYPE_LIST) {
            __listPrint(pointer.ptr);
        } 
        else if (pointer.type == TYPE_STRING){
            printf("\"");
            print(pointer, "", "");
            printf("\"");
        }
        else {
            print(pointer, "", "");
        }

        if (i < list->length - 1) {
            printf(", ");
        }
    }

    printf("]");
}

void listPrint(Clist *list)
{
    __listPrint(list);
    printf("\n");
}