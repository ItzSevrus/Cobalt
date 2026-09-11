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

> Detailed documentation and usage examples for Tony are available in [`docs/tony.md`](docs/tony.md).

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
└── Tony
    └── Tracked memory management
        ├── tPtr
        ├── Memory Pool
        ├── Allocation
        ├── Reallocation
        └── Deallocation
```

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

* [`Project Structure`](docs/project-structure.md) - Project's structure
* [`Tony`](docs/tony.md) — Cobalt's memory management system

As new components are added, their documentation will be provided in the same directory.

---

## 🗺️ Roadmap

Cobalt is still in its early development stage.

Planned areas include:

* Generic data structures
* Improved type handling
* Generic operations
* Additional memory management features
* Better error handling
* Expanded testing
* A stable public API
* First stable release

The roadmap is intentionally flexible as the architecture evolves.

---

## ⚠️ Development Status

**Cobalt is currently experimental.**

The API and internal architecture are subject to change.

The project is primarily being developed as an exploration of generic programming, memory management, data structures, and abstraction techniques in C.

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
