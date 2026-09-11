/*

    Author: Sahil Gour

    Licensed Under: MIT LICENSE

*/

#include "../../include/cobalt/io.h"
#include <stdio.h>
#include <string.h>

CFile fileOpen(const char *filepath, const char *mode)
{
    CFile cfile = {0};

    if (filepath == NULL || mode == NULL) {
        return cfile;
    }

    cfile.handle = fopen(filepath, mode);

    cfile.filepath = createString(filepath);
    cfile.mode = createString(mode);

    const char *lastSlash = strrchr(filepath, '/');

    if (lastSlash != NULL) {
        cfile.filename = createString(lastSlash + 1);
    } else {
        cfile.filename = createString(filepath);
    }

    return cfile;
}

CString fileRead(CFile *file)
{
    CString result = {0};

    if (file == NULL || file->handle == NULL) {
        return result;
    }

    if (fseek(file->handle, 0, SEEK_END) != 0) {
        return result;
    }

    long size = ftell(file->handle);

    if (size < 0) {
        return result;
    }

    if (fseek(file->handle, 0, SEEK_SET) != 0) {
        return result;
    }

    result.pointer = talloc((size_t)size + 1);

    if (result.pointer.ptr == NULL) {
        return result;
    }

    size_t bytesRead = fread(
        result.pointer.ptr,
        1,
        (size_t)size,
        file->handle
    );

    ((char *)result.pointer.ptr)[bytesRead] = '\0';

    result.pointer.type = TYPE_STRING;
    result.length = bytesRead;

    return result;
}

bool resetCursor(CFile *file)
{
    if (file == NULL || file->handle == NULL) {
        return false;
    }

    return fseek(file->handle, 0, SEEK_SET) == 0;
}

Clist fileReadLines(CFile *file)
{
    Clist result = createList();

    if (file == NULL || file->handle == NULL) {
        return result;
    }

    CString content = fileRead(file);

    if (content.pointer.ptr == NULL) {
        return result;
    }

    result = stringSplitBy(&content, "\n");

    destroy(content.pointer);

    return result;
}

CString fileReadLine(CFile *file)
{
    CString result = {0};

    if (file == NULL || file->handle == NULL) {
        return result;
    }

    size_t capacity = 64;
    size_t length = 0;

    tPtr buffer = talloc(capacity);

    if (buffer.ptr == NULL) {
        return result;
    }

    int character;

    while ((character = fgetc(file->handle)) != EOF) {

        if (character == '\n') {
            break;
        }

        if (length + 1 >= capacity) {

            capacity *= 2;

            tPtr resized = trealloc(buffer, capacity);

            if (resized.ptr == NULL) {
                destroy(buffer);
                return result;
            }

            buffer = resized;
        }

        ((char *)buffer.ptr)[length++] = (char)character;
    }

    /*
        No data was read and EOF was reached.
    */
    if (length == 0 && character == EOF) {
        destroy(buffer);
        return result;
    }

    ((char *)buffer.ptr)[length] = '\0';

    result.pointer = buffer;
    result.pointer.type = TYPE_STRING;
    result.length = length;

    return result;
}

bool fileClose(CFile *file)
{
    if (file == NULL || file->handle == NULL) {
        return false;
    }

    if (fclose(file->handle) != 0) {
        return false;
    }

    file->handle = NULL;

    return true;
}