#pragma once

#include "cobalt.h"
#include <stdbool.h>

tPtr talloc(size_t size);
bool destroy(tPtr tptr);
bool freeAll(void);
tPtr trealloc(tPtr pointer, size_t newSize);

void debug_mem_pool(void);