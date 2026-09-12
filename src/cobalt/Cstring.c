/*
    Author: Sahil Gour
    Licensed Under: MIT LICENSE
*/

#include "../../include/cobalt/Cstring.h"

#include <string.h>
#include <ctype.h>


size_t stringLength(CString *string)
{
    if (string == NULL || string->pointer.ptr == NULL) {
        return 0;
    }

    return string->length;
}


bool stringEquals(CString *a, CString *b)
{
    if (a == NULL || b == NULL) {
        return false;
    }

    if (a->pointer.ptr == NULL || b->pointer.ptr == NULL) {
        return false;
    }

    if (a->length != b->length) {
        return false;
    }

    return memcmp(
        a->pointer.ptr,
        b->pointer.ptr,
        a->length
    ) == 0;
}


bool stringContains(CString *string, CString *substring)
{
    if (string == NULL || substring == NULL) {
        return false;
    }

    if (string->pointer.ptr == NULL ||
        substring->pointer.ptr == NULL) {
        return false;
    }

    if (substring->length == 0) {
        return true;
    }

    if (substring->length > string->length) {
        return false;
    }

    char *source = string->pointer.ptr;
    char *target = substring->pointer.ptr;

    for (size_t i = 0;
         i <= string->length - substring->length;
         i++) {

        if (memcmp(
                source + i,
                target,
                substring->length
            ) == 0) {
            return true;
        }
    }

    return false;
}


CString stringTrim(CString *string)
{
    CString result = {0};

    if (string == NULL || string->pointer.ptr == NULL) {
        return result;
    }

    char *data = string->pointer.ptr;

    size_t start = 0;
    size_t end = string->length;

    while (start < end &&
           isspace((unsigned char)data[start])) {
        start++;
    }

    while (end > start &&
           isspace((unsigned char)data[end - 1])) {
        end--;
    }

    size_t newLength = end - start;

    result.pointer = talloc(newLength + 1);

    if (result.pointer.ptr == NULL) {
        return result;
    }

    memcpy(
        result.pointer.ptr,
        data + start,
        newLength
    );

    ((char *)result.pointer.ptr)[newLength] = '\0';

    result.pointer.type = TYPE_STRING;
    result.length = newLength;

    return result;
}


Clist stringSplit(CString *string)
{
    return stringSplitBy(string, " ");
}


Clist stringSplitBy(CString *string, const char *delimiter)
{
    Clist result = createList();

    if (string == NULL ||
        string->pointer.ptr == NULL ||
        delimiter == NULL ||
        delimiter[0] == '\0') {
        return result;
    }

    char *data = string->pointer.ptr;
    size_t delimiterLength = strlen(delimiter);

    size_t start = 0;

    for (size_t i = 0; i <= string->length;) {

        bool match = false;

        if (i + delimiterLength <= string->length) {
            match = memcmp(
                data + i,
                delimiter,
                delimiterLength
            ) == 0;
        }

        if (match || i == string->length) {

            size_t partLength = i - start;

            /*
                Create a CString even when the
                resulting part is empty.
            */
            tPtr object = talloc(sizeof(CString));

            if (object.ptr == NULL) {
                return result;
            }

            CString *piece = object.ptr;

            piece->pointer = talloc(partLength + 1);

            if (piece->pointer.ptr == NULL) {
                destroy(object);
                return result;
            }

            if (partLength > 0) {
                memcpy(
                    piece->pointer.ptr,
                    data + start,
                    partLength
                );
            }

            ((char *)piece->pointer.ptr)[partLength] = '\0';

            piece->pointer.type = TYPE_STRING;
            piece->length = partLength;

            if (!listAppend(&result, piece)) {
                destroy(piece->pointer);
                destroy(object);
                return result;
            }

            if (match) {
                i += delimiterLength;
            } else {
                break;
            }

            start = i;
            continue;
        }

        i++;
    }

    return result;
}