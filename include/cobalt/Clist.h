/*
    Author: Sahil Gour
    Licensed Under: MIT LICENSE
*/
#pragma once

#include "cobalt.h"
#include <stdbool.h>

bool __listAppend(Clist *list, void *data, DataType type);

#define listAppend(list, data) __listAppend(list, data, get_type(*data))

tPtr listGet(Clist *list, size_t index);
bool listRemove(Clist *list, size_t index);
tPtr listPop(Clist *list);

bool listSet(Clist *list, size_t index, tPtr *pointer);
bool listClear(Clist *list);

bool listSwap(Clist *list, size_t index_to_swap, size_t index_value_toswap);
void listReverse(Clist *list);

void listPrint(Clist *list);