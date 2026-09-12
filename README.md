# Cobalt

**A generic data and memory management system built in C.**

Cobalt is an experimental C library exploring how C's own features can be used to build higher-level abstractions while preserving the control, performance, and simplicity of C.

> **Cobalt is built to make C more capable — without hiding C underneath.**

---

## 🚀 Current Features

### Tony — Memory Management

**Tony** is Cobalt's memory management system.

It provides tracked memory allocation through **`tPtr`** handles, allowing memory to be:

* Allocated and registered with Tony
* Resized
* Individually destroyed
* Completely released
* Tracked through a dynamic memory pool

Tony provides direct lookup of allocations using the `tPtr.id`, which corresponds to the allocation's position in the memory pool.

```text
tPtr
 ├── id    → Pool index
 ├── ptr   → Allocated memory
 └── type  → Associated data type
```

Tony's public API currently consists of:

```c
tPtr talloc(size_t size);
tPtr trealloc(tPtr pointer, size_t newSize);
bool destroy(tPtr tptr);
bool freeAll(void);
```

Tony also provides:

```c
void debug_mem_pool(void);
```

for inspecting the current state of the memory pool.

> **Detailed documentation:** [`docs/tony.md`](docs/tony.md)

---

### CString — String Abstraction

**CString** is Cobalt's higher-level string abstraction.

It provides a structured representation of C strings while keeping their memory explicitly managed through Tony.

A `CString` contains:

```c
typedef struct {
    tPtr pointer;
    size_t length;
} CString;
```

CString currently supports:

* String creation
* Explicit length tracking
* String equality
* Substring searching
* Whitespace trimming
* String splitting
* Custom delimiter splitting
* Multi-character delimiters
* Integration with Clist
* Tony-managed string memory

Example:

```c
CString text = createString("Hello Cobalt");

printf("Length: %zu\n", stringLength(&text));
```

Strings can be split into a `Clist`:

```c
CString text = createString("Cobalt makes C easier");

Clist words = stringSplit(&text);

listPrint(&words);
```

Output:

```text
["Cobalt", "makes", "C", "easier"]
```

Custom delimiters are supported:

```c
CString data = createString("Sahil,20,Delhi,Cobalt");

Clist fields = stringSplitBy(&data, ",");

listPrint(&fields);
```

Output:

```text
["Sahil", "20", "Delhi", "Cobalt"]
```

> **Detailed documentation:** [`docs/Cstring.md`](docs/Cstring.md)

---

### Clist — Dynamic Data Lists

**Clist** is Cobalt's dynamic, heterogeneous list implementation.

Lists store references to `tPtr` objects managed by Tony, allowing a single List to contain different Cobalt data types as well as other Lists.

Clist currently supports:

* Dynamic list growth
* Heterogeneous data
* Nested Lists
* Element access
* Element removal
* Popping elements
* Replacing elements
* Clearing Lists
* Swapping elements
* Reversing Lists
* Recursive List printing

A simple example:

```c
Clist list = createList();

int value = 10;
CString name = createString("Sahil");

listAppend(&list, &value);
listAppend(&list, &name);

listPrint(&list);
```

Output:

```text
[10, "Sahil"]
```

Lists can also contain other Lists:

```text
[10, [3.14, "Sahil", ['X', [42, "Cobalt", 2.718000]]]]
```

Clist does not own the underlying allocations. Memory remains managed by Tony.

> **Detailed documentation:** [`docs/list.md`](docs/list.md)

---

### I/O — File Handling

**Cobalt I/O** provides a simple abstraction over C's standard file I/O facilities.

The I/O system is represented by `CFile`:

```c
typedef struct {

    FILE* handle;

    CString filepath;

    CString filename;

    CString mode;

} CFile;
```

Cobalt I/O currently supports:

* Opening files
* Closing files
* Reading entire files
* Reading individual lines
* Reading all lines
* Resetting the file cursor
* Basic file metadata

Example:

```c
CFile file = fileOpen(
    "data/test.txt",
    "r"
);
```

File metadata can be accessed through:

```c
file.filepath
file.filename
file.mode
```

The entire file can be read into a `CString`:

```c
CString content = fileRead(&file);

printString(&content);
```

Individual lines can be read incrementally:

```c
CString line = fileReadLine(&file);

printString(&line);
```

All lines can also be loaded into a `Clist`:

```c
Clist lines = fileReadLines(&file);

listPrint(&lines);
```

The file cursor can be reset with:

```c
resetCursor(&file);
```

and the file can be closed with:

```c
fileClose(&file);
```

Cobalt I/O uses the standard C `FILE*` internally while integrating dynamically allocated string data with Tony.

> **Detailed documentation:** [`docs/io.md`](docs/io.md)

---

### CSV — Comma-Separated Values

**Cobalt CSV** provides a lightweight layer for reading tabular data from CSV files.

The CSV system builds on top of Cobalt's existing `CFile`, `CString`, and `Clist` abstractions.

CSV currently supports:

* Reading individual CSV rows
* Reading all CSV rows
* Retrieving a column by its header name
* Displaying a formatted CSV preview
* Preserving empty CSV fields
* Restoring the file cursor after CSV queries and previews

The CSV API currently consists of:

```c
Clist csvReadRow(CFile *file);
Clist csvReadRows(CFile *file);
Clist csvGetByCol(CFile *file, CString *col_name);
void csvHead(CFile *file);
```

#### Reading a CSV Row

A single row can be read using:

```c
Clist row = csvReadRow(&file);

listPrint(&row);
```

For a CSV row:

```csv
Sahil,18,Delhi,Computer Science,87
```

the resulting list is:

```text
["Sahil", "18", "Delhi", "Computer Science", "87"]
```

CSV values are currently represented as `CString` objects.

#### Reading All Rows

The entire CSV can be loaded using:

```c
Clist rows = csvReadRows(&file);

listPrint(&rows);
```

The result is a nested `Clist`:

```text
[
    ["Name", "Age", "City"],
    ["Sahil", "18", "Delhi"],
    ["Ali", "19", "Mumbai"]
]
```

`csvReadRows()` resets the file cursor after reading.

#### Getting a Column

A column can be retrieved using its header name:

```c
CString column = createString("Name");

Clist names = csvGetByCol(
    &file,
    &column
);

listPrint(&names);
```

Output:

```text
["Sahil", "Ali", "Prateek", "Aarav"]
```

The function searches the first row for the requested column name and then retrieves that column from the remaining rows.

The original file cursor position is restored after the operation.

#### CSV Preview

`csvHead()` provides a formatted preview of the beginning of a CSV file:

```c
csvHead(&file);
```

Example:

```text
CSV: test.csv
LOCATED_AT: test/data/test.csv

Name      Age   City        Department               Marks
-------------------------------------------------------------
Sahil     18    Delhi       Computer Science         87
Ali       19    Mumbai      Information Technology   91
Prateek   20    Jaipur      Mechanical Engineering   76
```

The function displays up to **10 rows** and calculates column widths to format the output.

The original file cursor position is restored after the preview.

#### Empty Fields

Empty CSV fields are preserved.

For example:

```csv
Name,Age,City
Sahil,,Delhi
,19,
Ali,20,Mumbai
```

becomes:

```text
["Name", "Age", "City"]
["Sahil", "", "Delhi"]
["", "19", ""]
["Ali", "20", "Mumbai"]
```

This ensures that empty values do not shift subsequent columns.

#### Datatypes

The CSV layer currently treats values as text.

For example:

```csv
Age,Marks
18,87
19,91
20,76
```

is initially represented as:

```text
["18", "87"]
["19", "91"]
["20", "76"]
```

rather than automatically converting the values to C numeric types.

Datatype detection and higher-level data processing are intended for the **Dataset** layer.

> **Detailed documentation:** [`docs/csv.md`](docs/csv.md)

---

## 🧠 Design Philosophy

Cobalt is **not intended to replace C**.

Instead, the project explores how far C's own features can be pushed to provide useful abstractions while keeping the underlying language visible and controllable.

The project follows a few principles:

* **Keep C's control.**
* **Avoid unnecessary abstraction.**
* **Keep the public API simple.**
* **Make memory management explicit and predictable.**
* **Hide implementation complexity behind clean interfaces.**
* **Prefer reusable primitives over unnecessary language-like features.**

The goal is not to make C behave like another language.

The goal is to make **C more capable while remaining C.**

---

## 🧩 Current Architecture

Cobalt is currently being developed as a collection of reusable components.

```text
Cobalt
│
├── Tony
│   └── Tracked memory management
│       ├── tPtr
│       ├── Memory Pool
│       ├── Allocation
│       ├── Reallocation
│       └── Deallocation
│
├── CString
│   └── String abstraction
│       ├── String creation
│       ├── Length tracking
│       ├── Equality
│       ├── Substring search
│       ├── Trimming
│       └── Splitting
│
├── Clist
│   └── Dynamic heterogeneous lists
│       ├── Element references
│       ├── Dynamic growth
│       ├── Nested lists
│       ├── Mixed data types
│       └── List operations
│
├── I/O
│   └── File handling
│       ├── File opening
│       ├── File closing
│       ├── Full file reading
│       ├── Line reading
│       ├── Reading all lines
│       └── Cursor management
│
└── CSV
    └── CSV data handling
        ├── Row reading
        ├── Reading all rows
        ├── Column lookup
        ├── CSV preview
        └── Empty field preservation
```

The components work together:

```text
Tony
 │
 ├── CString
 │
 ├── Clist
 │
 └── I/O
       │
       ├── CString
       └── Clist
              │
              ▼
             CSV
              │
              ├── CString
              └── Clist
```

Tony provides the memory foundation, CString provides string handling, Clist provides heterogeneous collections, I/O provides file access, and CSV builds a simple tabular-data layer on top of them.

The architecture is actively evolving as the project develops.

Documentation for each component is provided inside the [`docs/`](docs/) directory.

---

## 🛠️ Building

Cobalt is currently under development.

Clone the repository:

```bash
git clone <repository-url>
cd Cobalt
```

Build the project:

```bash
make
```

The built libraries are placed inside:

```text
build/lib/
```

Cobalt currently produces:

```text
libcobalt_static.a
libcobalt_shared.so
```

> Build instructions may change while the project is under active development.

---

## 📚 Documentation

Documentation for individual Cobalt components is provided inside the [`docs/`](docs/) directory.

Current documentation:

* [`Project Structure`](docs/project-structure.md) — Project's structure
* [`Tony`](docs/tony.md) — Cobalt's memory management system
* [`CString`](docs/Cstring.md) — Cobalt's string abstraction
* [`Clist`](docs/list.md) — Cobalt's dynamic list datatype
* [`I/O`](docs/io.md) — Cobalt's file I/O system
* [`CSV`](docs/csv.md) — Cobalt's CSV reading and querying system

As new components are added, their documentation will be provided in the same directory.

---

## 🗺️ Roadmap

Cobalt is still in its early development stage.

Planned areas include:

* File writing
* File appending
* Generic data structures
* Improved type handling
* Generic operations
* Additional memory management features
* Dataset abstractions
* Data processing utilities
* Better error handling
* Expanded testing
* A stable public API
* First stable release

The roadmap is intentionally flexible as the architecture evolves.

---

## ⚠️ Development Status

**Cobalt is currently experimental.**

The API and internal architecture are subject to change.

The project is primarily being developed as an exploration of generic programming, memory management, data structures, string abstractions, file handling, CSV processing, and abstraction techniques in C.

It should not yet be considered a production-ready library.

---

## 🤝 Contributing

Cobalt is an open-source project.

Contributions, ideas, experiments, bug reports, and improvements are welcome.

If you find a problem or have an idea for improving the architecture, feel free to open an issue or submit a pull request.

---

## 📜 License

Cobalt is released under the **MIT License**.

See [`LICENSE`](LICENSE) for the complete license text.

---

## 💡 Why Cobalt?

C gives programmers a high degree of control, but building higher-level abstractions can require a significant amount of boilerplate.

Cobalt explores a middle ground:

```text
        Higher-level abstraction
                  ▲
                  │
               Cobalt
                  │
                  ▼
          Low-level C control
```

**Less boilerplate. More capability. Still C.**
