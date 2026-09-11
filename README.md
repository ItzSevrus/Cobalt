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

It provides a structured representation of a C string while keeping the underlying memory explicitly managed through Tony.

A `CString` contains:

```c
typedef struct {
    tPtr pointer;
    size_t length;
} CString;
```

The `length` field stores the number of characters in the string, excluding the terminating `'\0'`.

CString currently supports:

* String creation
* Explicit string length tracking
* String equality
* Substring searching
* Whitespace trimming
* Default string splitting
* Custom delimiter splitting
* Multi-character delimiters
* Preservation of empty fields
* Integration with Clist
* Tony-managed string memory

Example:

```c
CString string = createString("Hello Cobalt");

printf("Length: %zu\n", stringLength(&string));
```

CString operations can be used directly:

```c
CString a = createString("Cobalt");
CString b = createString("Cobalt");

printf("%s\n", stringEquals(&a, &b) ? "true" : "false");
```

String splitting produces a `Clist`:

```c
CString text = createString("Cobalt makes C easier");

Clist words = stringSplit(&text);

listPrint(&words);
```

Output:

```text
["Cobalt", "makes", "C", "easier"]
```

Custom delimiters are also supported:

```c
CString csv = createString("Sahil,20,Delhi,Cobalt");

Clist fields = stringSplitBy(&csv, ",");

listPrint(&fields);
```

Output:

```text
["Sahil", "20", "Delhi", "Cobalt"]
```

Multi-character delimiters are supported as well:

```c
CString data = createString("one::two::three::four");

Clist parts = stringSplitBy(&data, "::");

listPrint(&parts);
```

Output:

```text
["one", "two", "three", "four"]
```

Empty fields are preserved:

```text
"one,,three,"
```

produces:

```text
["one", "", "three", ""]
```

CString strings are backed by Tony-managed allocations, allowing them to participate naturally in Cobalt's memory management and data structures.

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

CString integrates directly with Clist, allowing strings to be stored alongside other Cobalt data types.

> **Detailed documentation:** [`docs/list.md`](docs/list.md)

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
│       ├── Splitting
│       └── Custom delimiters
│
└── Clist
    └── Dynamic heterogeneous lists
        ├── Element references
        ├── Dynamic growth
        ├── Nested lists
        ├── Mixed data types
        └── List operations
```

The current relationship between the components is:

```text
                Cobalt
                   │
        ┌──────────┴──────────┐
        │                     │
      Tony                CString
        │                     │
        │                     │
        └──────────┬──────────┘
                   │
                 Clist
```

Tony provides the memory foundation, CString provides a higher-level string abstraction, and Clist provides a heterogeneous container capable of storing Cobalt data types.

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

As new components are added, their documentation will be provided in the same directory.

---

## 🗺️ Roadmap

Cobalt is still in its early development stage.

Planned areas include:

* Expanded string operations
* Generic data structures
* Improved type handling
* Generic operations
* Additional memory management features
* File operations
* CSV parsing
* Dataset abstractions
* Data processing utilities
* Numerical computing
* Better error handling
* Expanded testing
* A stable public API
* First stable release

The roadmap is intentionally flexible as the architecture evolves.

---

## ⚠️ Development Status

**Cobalt is currently experimental.**

The API and internal architecture are subject to change.

The project is primarily being developed as an exploration of generic programming, memory management, data structures, string abstractions, and abstraction techniques in C.

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
