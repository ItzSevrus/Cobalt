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
    TYPE_UNKNOWN
} DataType;

typedef struct {
    size_t id;
    void* ptr;
    DataType type;
} tPtr;

#define get_type(x) _Generic((x), \
    int: TYPE_INT, \
    float: TYPE_FLOAT, \
    double: TYPE_DOUBLE, \
    char: TYPE_CHAR, \
    char*: TYPE_STRING, \
    FILE*: TYPE_FILE, \
    default: TYPE_UNKNOWN \
)