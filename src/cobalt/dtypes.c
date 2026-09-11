#include "../../include/cobalt/dtypes.h"
#include "../../include/cobalt/Clist.h"
#include <string.h>

Clist createList(void)
{
    Clist list = {0};

    return list;
}

CString createString(const char *string)
{
    CString result = {0};

    if (string == NULL) {
        return result;
    }

    size_t size = strlen(string) + 1;

    result.pointer = talloc(size);

    if (result.pointer.ptr == NULL) {
        return result;
    }

    memcpy(result.pointer.ptr, string, size);

    result.pointer.type = TYPE_STRING;
    result.length = size - 1;

    return result;
}

void printType(tPtr pointer){
    switch (pointer.type){
        case TYPE_INT:
            printf("<type 'int'>\n");
            break;
        case TYPE_DOUBLE:
            printf("<type 'double'>\n");
            break;
        case TYPE_FLOAT:
            printf("<type 'float'>\n");
            break;
        case TYPE_CHAR:
            printf("<type 'char'>\n");
            break;
        case TYPE_STRING:
            printf("<type 'string'>\n");
            break;
        case TYPE_FILE:
            printf("<type 'fileObject'>\n");
            break;
        case TYPE_LIST:
            printf("<type 'list'>\n");
            break;
        case TYPE_TPTR:
            printf("<type 'tptr'>\n");
            break;
        case TYPE_UNKNOWN:
        default:
            printf("<type 'unknown'>\n");
            break;
    }
}

void printStringInline(CString *string)
{
    if (string == NULL || string->pointer.ptr == NULL) {
        return;
    }

    printf("%s", (char *)string->pointer.ptr);
}

void printString(CString *string){
    if (string == NULL || string->pointer.ptr == NULL) {
        return;
    }
    printf("%s\n", (char *)string->pointer.ptr);
}

void __print(tPtr pointer, const char *sep, const char *end)
{
    if (pointer.ptr == NULL) {
        printf("<null>%s", end);
        return;
    }
    switch (pointer.type) {
        case TYPE_INT:
            printf("%d%s", *(int *)pointer.ptr, sep);
            break;

        case TYPE_FLOAT:
            printf("%f%s", *(float *)pointer.ptr, sep);
            break;

        case TYPE_DOUBLE:
            printf("%lf%s", *(double *)pointer.ptr, sep);
            break;

        case TYPE_CHAR:
            printf("\'%c\'%s", *(char *)pointer.ptr, sep);
            break;

        case TYPE_STRING:
            printStringInline((CString *)pointer.ptr);
            break;

        case TYPE_LIST:
            /* List printing will be implemented here */
            break;

        case TYPE_FILE:
            /* File printing will be implemented here */
            break;

        default:
            printf("<unknown>%s", sep);
            break;
    }

    printf("%s", end);
}