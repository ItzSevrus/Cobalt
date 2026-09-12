/*

    Author: Sahil Gour

    Licensed Under: MIT LICENSE

*/
#include "../../include/cobalt/csv.h"

Clist csvReadRow(CFile *file)
{
    Clist result = createList();

    if (file == NULL || file->handle == NULL) {
        return result;
    }

    CString line = fileReadLine(file);

    if (line.pointer.ptr == NULL) {
        return result;
    }

    result = stringSplitBy(&line, ",");

    destroy(line.pointer);

    return result;
}

Clist csvReadRows(CFile *file)
{
    Clist result = createList();

    if (file == NULL || file->handle == NULL) {
        return result;
    }

    while (true) {

        Clist row = csvReadRow(file);

        /*
            An empty list means EOF.
        */
        if (row.length == 0) {
            break;
        }

        Clist *rowPtr = talloc(sizeof(Clist)).ptr;

        if (rowPtr == NULL) {
            return result;
        }

        *rowPtr = row;

        if (!listAppend(&result, rowPtr)) {
            destroy(rowPtr->items);
            return result;
        }
    }

    resetCursor(file);
    return result;
}

Clist csvGetByCol(CFile *file, CString *col_name)
{
    Clist result = createList();

    if (file == NULL ||
        file->handle == NULL ||
        col_name == NULL ||
        col_name->pointer.ptr == NULL) {
        return result;
    }

    long position = ftell(file->handle);

    if (position < 0) {
        return result;
    }

    /*
        Read header.
    */
    Clist header = csvReadRow(file);

    if (header.length == 0) {
        fseek(file->handle, position, SEEK_SET);
        return result;
    }

    /*
        Find requested column.
    */
    size_t columnIndex = 0;
    bool found = false;

    for (size_t i = 0; i < header.length; i++) {

        tPtr value = listGet(&header, i);

        if (value.type != TYPE_STRING) {
            continue;
        }

        CString *name = value.ptr;

        if (stringEquals(name, col_name)) {
            columnIndex = i;
            found = true;
            break;
        }
    }

    if (!found) {
        fseek(file->handle, position, SEEK_SET);
        return result;
    }

    /*
        Read remaining rows.
    */
    while (true) {

        Clist row = csvReadRow(file);

        if (row.length == 0) {
            break;
        }

        if (columnIndex >= row.length) {
            continue;
        }

        tPtr value = listGet(&row, columnIndex);

        if (value.ptr == NULL) {
            continue;
        }

        if (!__listAppend(&result, value.ptr, TYPE_STRING)) {
            break;
        }
    }

    /*
        Restore original cursor.
    */
    fseek(file->handle, position, SEEK_SET);

    return result;
}

void csvHead(CFile *file)
{
    if (file == NULL || file->handle == NULL) {
        return;
    }

    long position = ftell(file->handle);

    if (position < 0) {
        return;
    }
    
    printf("\n");
    printf("CSV: ");
    printString(&file->filename);
    printf("LOCATED_AT: ");
    printString(&file->filepath);

    /*
        Store up to 10 rows.
    */
    tPtr rowsPtr = talloc(10 * sizeof(Clist));

    if (rowsPtr.ptr == NULL) {
        return;
    }

    Clist *rows = rowsPtr.ptr;
    size_t rowCount = 0;

    /*
        Read up to 10 rows.
    */
    for (size_t i = 0; i < 10; i++) {

        Clist row = csvReadRow(file);

        if (row.length == 0) {
            break;
        }

        rows[rowCount++] = row;
    }

    if (rowCount == 0) {
        fseek(file->handle, position, SEEK_SET);
        return;
    }

    /*
        Determine number of columns from header.
    */
    size_t columns = rows[0].length;

    /*
        Store maximum width of each column.
    */
    tPtr widthsPtr = talloc(columns * sizeof(size_t));

    if (widthsPtr.ptr == NULL) {
        fseek(file->handle, position, SEEK_SET);
        return;
    }

    size_t *widths = widthsPtr.ptr;

    for (size_t i = 0; i < columns; i++) {
        widths[i] = 0;
    }

    /*
        Find maximum width for every column.
    */
    for (size_t i = 0; i < rowCount; i++) {

        for (size_t j = 0;
             j < rows[i].length && j < columns;
             j++) {

            tPtr value = listGet(&rows[i], j);

            if (value.type == TYPE_STRING) {

                CString *string = value.ptr;

                if (string->length > widths[j]) {
                    widths[j] = string->length;
                }
            }
        }
    }

    /*
        Print table.
    */
    for (size_t i = 0; i < rowCount; i++) {

        for (size_t j = 0; j < columns; j++) {

            size_t currentWidth = 0;

            if (j < rows[i].length) {

                tPtr value = listGet(&rows[i], j);

                print(value, "", "");

                if (value.type == TYPE_STRING) {
                    currentWidth =
                        ((CString *)value.ptr)->length;
                }
            }

            /*
                Don't pad the final column.
            */
            if (j < columns - 1) {

                for (size_t k = currentWidth;
                     k < widths[j] + 3;
                     k++) {

                    printf(" ");
                }
            }
        }

        printf("\n");

        /*
            Separator after header.
        */
        if (i == 0) {

            for (size_t j = 0; j < columns; j++) {

                for (size_t k = 0;
                     k < widths[j] + 3;
                     k++) {

                    printf("-");
                }
            }

            printf("\n");
        }
    }

    /*
        Restore original cursor position.
    */
    fseek(file->handle, position, SEEK_SET);
}