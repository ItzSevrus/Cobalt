#pragma once

#include "cobalt.h"

CString createString(const char *string);

size_t stringLength(CString *string);

bool stringEquals(CString *a, CString *b);

bool stringContains(CString *string, CString *substring);

CString stringTrim(CString *string);

Clist stringSplit(CString *string);

Clist stringSplitBy(CString *string, const char *delimiter);