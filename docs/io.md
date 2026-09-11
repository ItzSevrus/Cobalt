# Cobalt I/O

Cobalt I/O provides a simple abstraction over C's standard file I/O facilities.

The goal of the I/O component is not to replace C's `FILE` API, but to provide a Cobalt-friendly interface that integrates file operations with `CString`, `Clist`, and Tony's memory management system.

---

## Overview

The Cobalt I/O abstraction is represented by:

```c
typedef struct {

    FILE* handle;

    CString filepath;

    CString filename;

    CString mode;

} CFile;
```

A `CFile` contains:

* The underlying C `FILE*` handle
* The complete file path
* The file name
* The mode used to open the file

Conceptually:

```text
CFile
│
├── handle
│   └── FILE*
│
├── filepath
│   └── CString
│
├── filename
│   └── CString
│
└── mode
    └── CString
```

The underlying file stream is still managed by the C standard library, while the Cobalt string metadata is represented using `CString`.

---

# Opening Files

Files are opened using:

```c
CFile fileOpen(const char *filepath, const char *mode);
```

Example:

```c
CFile file = fileOpen(
    "data/test.txt",
    "r"
);
```

The function internally uses C's:

```c
fopen()
```

to create the underlying file stream.

The resulting `CFile` contains:

```text
filepath → "data/test.txt"
filename → "test.txt"
mode     → "r"
handle   → FILE*
```

---

## Filepath

The complete path supplied to `fileOpen()` is stored in:

```c
CString filepath;
```

For example:

```text
/home/sahil/data/people.csv
```

is stored as:

```text
filepath = "/home/sahil/data/people.csv"
```

---

## Filename

The filename is extracted from the final `/` in the supplied path.

For:

```text
/home/sahil/data/people.csv
```

the resulting filename is:

```text
people.csv
```

If no `/` exists, the entire supplied path is treated as the filename.

For example:

```text
people.csv
```

results in:

```text
filename = "people.csv"
```

---

## File Mode

The mode passed to `fopen()` is stored in:

```c
CString mode;
```

Examples include:

```text
"r"
"w"
"a"
"r+"
"w+"
"a+"
```

The mode is preserved as a Cobalt string so that the `CFile` structure contains information about how the file was opened.

---

# Reading Files

Cobalt provides two different styles of reading.

## Reading the Entire File

```c
CString fileRead(CFile *file);
```

`fileRead()` reads the entire contents of the opened file and returns them as a `CString`.

Example:

```c
CFile file = fileOpen("data.txt", "r");

CString content = fileRead(&file);

printString(&content);
```

The operation can be visualized as:

```text
FILE*
 │
 │ fread()
 ▼
Tony-managed memory
 │
 ▼
CString
```

The returned `CString` contains the file contents and its length.

---

## File Cursor

File reading is cursor-based.

When data is read from a `FILE*`, the cursor advances through the file.

For example:

```text
Cobalt
Tony
CString
```

Initially:

```text
Cobalt
^
cursor
```

After reading `"Cobalt"`:

```text
Cobalt
      ^
      cursor
Tony
CString
```

Cobalt provides `resetCursor()` to move the cursor back to the beginning.

---

# Resetting the Cursor

```c
bool resetCursor(CFile *file);
```

Example:

```c
resetCursor(&file);
```

Internally, this resets the underlying stream using:

```c
fseek(file->handle, 0, SEEK_SET);
```

A successful reset returns:

```c
true
```

Otherwise:

```c
false
```

Example:

```c
CString first = fileRead(&file);

resetCursor(&file);

CString second = fileRead(&file);
```

Both reads will start from the beginning of the file.

---

# Reading Lines

Cobalt provides:

```c
CString fileReadLine(CFile *file);
```

This reads a single line from the current file cursor position.

The function continues reading until either:

```text
'\n'
```

or:

```text
EOF
```

is encountered.

The newline character itself is not included in the returned `CString`.

Example file:

```text
Hello Cobalt
Tony manages memory.
CString manages strings.
```

Code:

```c
CString line = fileReadLine(&file);

printString(&line);
```

Result:

```text
Hello Cobalt
```

Calling it again:

```c
line = fileReadLine(&file);
```

returns:

```text
Tony manages memory.
```

---

## Line Buffer Growth

`fileReadLine()` uses Tony for its dynamically growing buffer.

The initial allocation is:

```text
64 bytes
```

If the line becomes too large, the buffer is expanded using:

```c
trealloc()
```

Conceptually:

```text
talloc(64)
     │
     ▼
read characters
     │
     ▼
buffer full
     │
     ▼
trealloc(128)
     │
     ▼
continue reading
```

This allows `fileReadLine()` to handle lines larger than its initial buffer size.

The final Tony-managed allocation becomes the memory backing the returned `CString`.

---

# Reading All Lines

Cobalt also provides:

```c
Clist fileReadLines(CFile *file);
```

This reads the file and returns its contents as a `Clist` of `CString` values.

Example:

```c
Clist lines = fileReadLines(&file);

listPrint(&lines);
```

For a file containing:

```text
Cobalt
Tony
CString
Clist
```

the resulting list is conceptually:

```text
["Cobalt", "Tony", "CString", "Clist"]
```

This makes `fileReadLines()` useful when the entire file needs to be represented as a Cobalt collection.

---

# Streaming Line-by-Line

`fileReadLine()` can also be used for streaming through a file.

Example:

```c
while (1) {

    CString line = fileReadLine(&file);

    if (line.pointer.ptr == NULL) {
        break;
    }

    printString(&line);

    destroy(line.pointer);
}
```

The important difference is that the entire file does not need to be loaded into memory at once.

The process becomes:

```text
File
 │
 ├── Line 1 → CString
 ├── Line 2 → CString
 ├── Line 3 → CString
 ├── Line 4 → CString
 └── EOF
```

This is particularly useful for large text files.

---

# Closing Files

Files are closed using:

```c
bool fileClose(CFile *file);
```

Example:

```c
fileClose(&file);
```

Internally, the underlying stream is closed using:

```c
fclose(file->handle);
```

After successfully closing the file, Cobalt sets:

```c
file->handle = NULL;
```

This allows the `CFile` to represent its closed state.

Conceptually:

```text
Before:

CFile
└── handle → FILE*


After:

CFile
└── handle → NULL
```

Calling `fileClose()` on a `NULL` or already-closed handle returns:

```c
false
```

---

# Memory Management

Cobalt I/O integrates with Tony for dynamically allocated string data.

The relationship is:

```text
CFile
│
├── FILE*
│   └── Managed by C standard library
│
├── filepath
│   └── CString → Tony
│
├── filename
│   └── CString → Tony
│
└── mode
    └── CString → Tony
```

The `FILE*` itself is **not a Tony allocation**.

It belongs to the C standard library and is released using:

```c
fclose()
```

Cobalt's dynamically allocated string data remains part of Tony's memory system.

---

# File Cursor Model

Cobalt keeps the standard C file cursor model.

For example:

```c
CString first = fileReadLine(&file);
CString second = fileReadLine(&file);
```

results in:

```text
File
│
├── first line
│       ↓
│    readLine()
│
├── second line
│       ↓
│    readLine()
│
└── remaining data
        ↓
      cursor
```

To return to the beginning:

```c
resetCursor(&file);
```

This makes repeated reading possible without reopening the file.

---

# Complete Example

```c
#include "cobalt/io.h"

#include <stdio.h>

int main(void)
{
    CFile file = fileOpen(
        "test/data/test.txt",
        "r"
    );

    if (file.handle == NULL) {
        printf("Failed to open file.\n");
        return 1;
    }

    printf("Filepath: ");
    printString(&file.filepath);

    printf("Filename: ");
    printString(&file.filename);

    printf("Mode: ");
    printString(&file.mode);

    printf("\nReading first line:\n");

    CString line = fileReadLine(&file);

    printString(&line);

    destroy(line.pointer);

    resetCursor(&file);

    printf("\nReading entire file:\n");

    CString content = fileRead(&file);

    printString(&content);

    destroy(content.pointer);

    fileClose(&file);

    freeAll();

    return 0;
}
```

---

# Current API

The current Cobalt I/O API consists of:

```c
CFile fileOpen(const char *filepath, const char *mode);

CString fileRead(CFile *file);

CString fileReadLine(CFile *file);

Clist fileReadLines(CFile *file);

bool resetCursor(CFile *file);

bool fileClose(CFile *file);
```

---

# Design Philosophy

Cobalt I/O intentionally provides a thin layer over C's existing file facilities.

It does not attempt to hide the underlying `FILE*`.

Instead:

```text
Cobalt I/O
     │
     ▼
Standard C FILE*
```

The purpose of the abstraction is to integrate file operations with the rest of Cobalt:

```text
Tony
 │
 ├── CString
 │
 └── Clist
       ▲
       │
     File
```

This allows file contents to move naturally through Cobalt's existing abstractions.

For example:

```text
File
 ↓
CString
 ↓
Clist
```

can later become:

```text
File
 ↓
CSV
 ↓
Clist
 ↓
Dataset
```

The I/O layer therefore acts as the foundation for Cobalt's future data-processing functionality.

---

# Current Limitations

The current I/O implementation is intentionally small.

It currently focuses on:

* Opening files
* Closing files
* Reading entire files
* Reading individual lines
* Reading all lines
* Resetting the file cursor
* Basic file metadata

The following functionality is not currently part of the I/O layer:

* File writing
* File appending
* Random-access helpers
* File copying
* File deletion
* Directory handling
* File metadata from the operating system
* Encoding conversion
* CSV-specific parsing

These can be added as the Cobalt architecture develops.

---

# Summary

Cobalt I/O provides a simple bridge between C's standard file system and Cobalt's higher-level abstractions.

```text
             Cobalt I/O
                  │
          ┌───────┴───────┐
          │               │
       FILE*           CString
          │               │
          └───────┬───────┘
                  │
                Clist
```

The current design intentionally remains small.

The goal is to provide useful file primitives without turning Cobalt's I/O layer into a replacement for the C standard library.

**Simple files. Explicit control. Cobalt abstractions.**
