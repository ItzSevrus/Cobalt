/*
    Author: Sahil Gour
    Licensed Under: MIT LICENSE
*/
#pragma once
#include <stdio.h>

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_LIST,
    TYPE_FILE,
    TYPE_TPTR,
    TYPE_UNKNOWN
} DataType;

typedef struct {
    size_t id;
    void* ptr;
    DataType type;
} tPtr;

typedef struct {
    size_t length;
    tPtr items;
} Clist;

#define get_type(x) _Generic((x), \
    int: TYPE_INT, \
    float: TYPE_FLOAT, \
    double: TYPE_DOUBLE, \
    char: TYPE_CHAR, \
    CString: TYPE_STRING, \
    FILE*: TYPE_FILE, \
    Clist: TYPE_LIST, \
    tPtr: TYPE_TPTR, \
    default: TYPE_UNKNOWN \
)

typedef struct {
    tPtr pointer;
    size_t length;
} CString;

Clist createList(void);
CString createString(const char *string);

void printType(tPtr pointer);
void __print(tPtr pointer, const char *sep, const char *end);

#define __print1(pointer) __print(pointer, "", "\n")
#define __print2(pointer, sep) __print(pointer, sep, "\n")
#define __print3(pointer, sep, end) __print(pointer, sep, end)

#define __print_select(_1, _2, _3, NAME, ...) NAME

#define print(...) \
    __print_select(__VA_ARGS__, __print3, __print2, __print1)(__VA_ARGS__)

void printString(CString *string);