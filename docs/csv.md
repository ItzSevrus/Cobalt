# CSV

Cobalt provides a lightweight CSV layer built on top of its existing `CFile`, `CString`, and `Clist` abstractions.

The CSV module provides basic functionality for:

* Reading a single CSV row
* Reading all rows from a CSV file
* Retrieving a column by its name
* Displaying the first rows of a CSV file in a formatted table

The CSV layer intentionally keeps CSV values as `CString` objects. **Datatype inference is not performed by the CSV module.** Higher-level datatype detection belongs to the Dataset layer.

---

## Overview

The CSV API is defined in:

```c
#include <cobalt/csv.h>
```

The module depends on Cobalt's file, string, list, and memory abstractions.

Conceptually:

```text
CFile
  │
  ▼
CSV
  │
  ├── csvReadRow()
  ├── csvReadRows()
  ├── csvGetByCol()
  └── csvHead()
```

CSV data is represented using `Clist` objects.

A row such as:

```csv
Sahil,18,Delhi,Computer Science,87
```

is represented approximately as:

```text
[
    "Sahil",
    "18",
    "Delhi",
    "Computer Science",
    "87"
]
```

Each value is currently stored as a `CString`.

---

# Functions

## `csvReadRow()`

```c
Clist csvReadRow(CFile *file);
```

Reads one CSV row from the current position of a `CFile`.

### Parameters

| Parameter | Type      | Description   |
| --------- | --------- | ------------- |
| `file`    | `CFile *` | Open CSV file |

### Return value

Returns a `Clist` containing the values from the next CSV row.

For example:

```csv
Name,Age,City
```

produces:

```text
["Name", "Age", "City"]
```

If the file is invalid, closed, or the end of the file is reached, an empty `Clist` is returned.

### Example

```c
CFile file = fileOpen("data.csv", "r");

Clist row = csvReadRow(&file);

listPrint(&row);
```

Output:

```text
["Name", "Age", "City"]
```

The file cursor is advanced after reading the row.

---

## `csvReadRows()`

```c
Clist csvReadRows(CFile *file);
```

Reads all CSV rows from the current file.

The returned `Clist` contains a `Clist` for every row.

Conceptually:

```text
[
    ["Name", "Age", "City"],
    ["Sahil", "18", "Delhi"],
    ["Ali", "19", "Mumbai"]
]
```

### Parameters

| Parameter | Type      | Description   |
| --------- | --------- | ------------- |
| `file`    | `CFile *` | Open CSV file |

### Return value

Returns a `Clist` containing all successfully read rows.

If the file is invalid or closed, an empty `Clist` is returned.

### Example

```c
CFile file = fileOpen("data.csv", "r");

Clist rows = csvReadRows(&file);

listPrint(&rows);
```

Example output:

```text
[["Name", "Age", "City"],
 ["Sahil", "18", "Delhi"],
 ["Ali", "19", "Mumbai"]]
```

### Cursor behavior

`csvReadRows()` resets the file cursor after reading.

Therefore:

```c
Clist rows = csvReadRows(&file);
```

does not leave the file at EOF.

The file is reset using:

```c
resetCursor(file);
```

after the rows have been read.

---

# `csvGetByCol()`

```c
Clist csvGetByCol(CFile *file, CString *col_name);
```

Retrieves all values belonging to a specified CSV column.

The column is identified using the value in the CSV header.

For example:

```csv
Name,Age,City
Sahil,18,Delhi
Ali,19,Mumbai
Prateek,20,Jaipur
```

Calling:

```c
CString column = createString("Name");

Clist names = csvGetByCol(&file, &column);
```

produces:

```text
["Sahil", "Ali", "Prateek"]
```

### Parameters

| Parameter  | Type        | Description                    |
| ---------- | ----------- | ------------------------------ |
| `file`     | `CFile *`   | Open CSV file                  |
| `col_name` | `CString *` | Name of the column to retrieve |

### Return value

Returns a `Clist` containing the values of the requested column.

If:

* `file` is `NULL`
* the file is not open
* `col_name` is `NULL`
* the column does not exist
* the CSV header cannot be read

an empty `Clist` is returned.

### Example

```c
CFile file = fileOpen("data.csv", "r");

CString column = createString("Name");

Clist names = csvGetByCol(&file, &column);

listPrint(&names);
```

Output:

```text
["Sahil", "Ali", "Prateek", "Aarav"]
```

### Column lookup

The function first reads the header row and searches for a matching column name.

For:

```csv
Name,Age,City,Marks
Sahil,18,Delhi,87
Ali,19,Mumbai,91
```

the column indexes are:

```text
Name  → 0
Age   → 1
City  → 2
Marks → 3
```

Requesting:

```c
CString column = createString("City");
```

causes the function to locate `City` at index `2` and retrieve that position from every subsequent row.

### Cursor behavior

`csvGetByCol()` preserves the file cursor position.

Before performing the query, the current position is saved using:

```c
long position = ftell(file->handle);
```

After the operation, the original position is restored:

```c
fseek(file->handle, position, SEEK_SET);
```

This allows column queries to be performed without permanently changing the caller's current file position.

---

# `csvHead()`

```c
void csvHead(CFile *file);
```

Displays a formatted preview of the beginning of a CSV file.

The function displays **up to 10 rows**, including the header if the CSV contains one.

It also displays basic file information:

```text
CSV: test.csv
LOCATED_AT: test/data/test.csv
```

### Example

For a CSV such as:

```csv
Name,Age,City,Department,Marks
Sahil,18,Delhi,Computer Science,87
Ali,19,Mumbai,Information Technology,91
Prateek,20,Jaipur,Mechanical Engineering,76
```

`csvHead()` produces a formatted table similar to:

```text
CSV: test.csv
LOCATED_AT: test/data/test.csv

Name      Age   City        Department               Marks
-------------------------------------------------------------
Sahil     18    Delhi       Computer Science         87
Ali       19    Mumbai      Information Technology   91
Prateek   20    Jaipur      Mechanical Engineering   76
```

### Parameters

| Parameter | Type      | Description   |
| --------- | --------- | ------------- |
| `file`    | `CFile *` | Open CSV file |

### Return value

`csvHead()` does not return a value.

```c
void
```

If the file is invalid or closed, the function simply returns.

---

## How `csvHead()` formats the table

`csvHead()` performs several steps.

### 1. Save the cursor

The current file position is saved:

```c
long position = ftell(file->handle);
```

This allows the function to restore the file afterwards.

### 2. Display file information

The filename and filepath stored by `CFile` are displayed:

```text
CSV: test.csv
LOCATED_AT: test/data/test.csv
```

### 3. Read up to 10 rows

The function allocates storage for ten `Clist` objects and repeatedly calls:

```c
csvReadRow(file);
```

until either:

* 10 rows have been read, or
* EOF is reached.

### 4. Calculate column widths

The maximum string length for each column is calculated.

For example:

```text
Name
Sahil
Prateek
Abhishek
```

results in the column width being determined by the longest value.

### 5. Print the table

The rows are printed using Cobalt's generic:

```c
print()
```

function.

Padding is added between columns to create a readable table.

### 6. Print the header separator

A separator is printed after the first row:

```text
Name      Age   City
---------------------
```

### 7. Restore the cursor

Finally, the original file position is restored:

```c
fseek(file->handle, position, SEEK_SET);
```

Therefore, calling `csvHead()` does not permanently consume CSV data.

---

# Empty CSV Fields

CSV columns can contain empty values.

Cobalt's CSV layer preserves these fields.

For example:

```csv
Name,Age,City
Sahil,,Delhi
,19,
Ali,20,Mumbai
```

is represented as:

```text
["Name", "Age", "City"]
["Sahil", "", "Delhi"]
["", "19", ""]
["Ali", "20", "Mumbai"]
```

This behavior is important because the position of an empty field still represents a column.

For example:

```csv
Sahil,,Delhi
```

must not become:

```text
["Sahil", "Delhi"]
```

because that would incorrectly move `Delhi` into the `Age` column.

Instead it remains:

```text
["Sahil", "", "Delhi"]
```

This behavior is provided by `stringSplitBy()`.

---

# CSV Values and Datatypes

The CSV module does **not** determine whether a value represents an integer, floating-point number, or another datatype.

For example:

```csv
Name,Age,Marks
Sahil,18,87
Ali,19,91
```

the values are initially represented as strings:

```text
"18"
"19"
"87"
"91"
```

They are not automatically converted into:

```text
18
19
87
91
```

This is intentional.

The CSV layer is responsible for reading the textual representation of the data.

Datatype inference belongs to the higher-level **Dataset** abstraction.

Conceptually:

```text
CSV
 │
 │  Read textual values
 ▼
Clist / CString
 │
 ▼
Dataset
 │
 │  Infer datatypes
 ▼
Typed data
```

This separation keeps the CSV implementation lightweight while allowing Dataset to provide more advanced functionality.

---

# Memory Management

CSV uses Cobalt's memory-management system through Tony.

Rows and their values are represented using Cobalt structures such as:

```c
Clist
CString
tPtr
```

and are backed by Cobalt-managed allocations.

For example, `csvReadRow()` reads a line into a `CString`, splits it into values, and then destroys the temporary line buffer:

```c
destroy(line.pointer);
```

The resulting values remain available through the returned `Clist`.

Users should eventually release Cobalt-managed allocations using:

```c
freeAll();
```

when they are finished with the data.

Example:

```c
CFile file = fileOpen("data.csv", "r");

Clist rows = csvReadRows(&file);

listPrint(&rows);

fileClose(&file);

freeAll();
```

---

# Example

A simple CSV program:

```c
#include <cobalt/cobalt.h>

int main(void)
{
    CFile file = fileOpen("data.csv", "r");

    if (file.handle == NULL) {
        return 1;
    }

    csvHead(&file);

    Clist rows = csvReadRows(&file);

    printf("Rows: %zu\n", rows.length);

    CString column = createString("Name");

    Clist names = csvGetByCol(&file, &column);

    listPrint(&names);

    fileClose(&file);

    freeAll();

    return 0;
}
```

Possible output:

```text
CSV: data.csv
LOCATED_AT: data.csv

Name      Age   City
----------------------
Sahil     18    Delhi
Ali       19    Mumbai
Prateek   20    Jaipur

Rows: 4

["Sahil", "Ali", "Prateek"]
```

---

# Current CSV Scope

The current CSV implementation provides basic CSV functionality:

* Row reading
* Full CSV reading
* Column lookup by header name
* CSV preview
* Preservation of empty fields
* File cursor preservation for queries and previews
* Integration with `CFile`, `CString`, and `Clist`

The current implementation treats CSV as a **simple comma-separated text format**.

It does not currently provide:

* Automatic datatype inference
* Numeric conversion
* Dataset operations
* CSV writing
* Column filtering
* Row filtering
* Statistical operations
* Advanced CSV dialect configuration
* Schema management

These features can be implemented at higher layers of Cobalt.

In particular, datatype inference and data-oriented operations are intended to belong to the **Dataset** layer.

---

# API Summary

| Function        | Purpose                                      |
| --------------- | -------------------------------------------- |
| `csvReadRow()`  | Read one CSV row                             |
| `csvReadRows()` | Read all CSV rows                            |
| `csvGetByCol()` | Retrieve a column by name                    |
| `csvHead()`     | Display a formatted preview of up to 10 rows |

---

# Design

The CSV module sits above Cobalt's basic abstractions:

```text
                    Cobalt
                       │
          ┌────────────┼────────────┐
          ▼            ▼            ▼
       CString       Clist        CFile
          │            │            │
          └────────────┼────────────┘
                       ▼
                      CSV
                       │
          ┌────────────┼────────────┐
          ▼            ▼            ▼
     csvReadRow   csvReadRows   csvGetByCol
                       │
                       ▼
                    csvHead
                       │
                       ▼
                    Dataset
```

The goal is to keep each layer focused.

**CSV handles CSV reading. Dataset handles structured data.**

This allows Cobalt to build higher-level data functionality without making the CSV parser itself unnecessarily complex.
