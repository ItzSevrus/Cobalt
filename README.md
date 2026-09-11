# Cobalt

**A dynamic type and generic data system built in C.**

Cobalt is an experimental C library focused on making C more flexible without taking away the control, performance, and simplicity that make C powerful.

The project provides a unified `var` abstraction capable of representing different types of values while using C's compile-time capabilities to automatically detect types.

> **Cobalt is built to bring higher-level data handling to C — without hiding C underneath.**

---

## 🚀 Features

### Dynamic Type Detection

Cobalt uses C11 `_Generic` to detect the type of an expression at compile time.

```c
get_type(10);       // TYPE_INT
get_type(3.14f);    // TYPE_FLOAT
get_type(3.14);     // TYPE_DOUBLE
get_type('A');      // TYPE_CHAR
get_type("Hello");  // TYPE_STRING
```

Cobalt can also recognize custom Cobalt types such as `List` and other library-defined types.

---

## 🧠 Design Philosophy

Cobalt is **not intended to replace C**.

Instead, it explores how far C's own features can be pushed to provide abstractions normally associated with higher-level languages.

The project follows a few principles:

* **Keep C's control.**
* **Avoid unnecessary abstraction.**
* **Keep the public API simple.**
* **Use compile-time mechanisms whenever possible.**
* **Make memory management explicit and predictable.**
* **Hide implementation complexity behind clean interfaces.**
* **Prefer reusable primitives over language-like magic.**

The goal is not to make C behave exactly like another language.

The goal is to make **C more capable while remaining C.**

---

## 🧩 Current Architecture

Cobalt is being developed as a collection of reusable components.

```text
Cobalt
│
├── Type System
│   └── Automatic type detection
├── Data Structures
│   ├── List
│   └── Matrix
│
└── Utilities
    └── Supporting functionality
```

The architecture is actively evolving as the project develops.

---

## 🛠️ Building

Cobalt is currently under development.

Clone the repository:

```bash
git clone <repository-url>
cd cobalt
```

Build using the project's build system:

```bash
make build
```

You will find built **cobalt.so** inside **build/lib** directory.

> Build instructions may change while the project is under active development.

---

## 📋 Supported Types

Current type detection includes:

| Type     | Cobalt Type   |
| -------- | ------------- |
| `int`    | `TYPE_INT`    |
| `float`  | `TYPE_FLOAT`  |
| `double` | `TYPE_DOUBLE` |
| `char`   | `TYPE_CHAR`   |
| `char*`  | `TYPE_STRING` |
| `String` | `TYPE_STRING` |
| `List`   | `TYPE_LIST`   |
| `FILE*`  | `TYPE_FILE`   |

Additional types and abstractions will be added as development continues.

---

## 🗺️ Roadmap

* [ ] Compile-time type detection
* [ ] Basic datatype system
* [ ] Initial generic data structures
* [ ] Simplify pointer handling
* [ ] Improve memory management
* [ ] Generic arithmetic operations
* [ ] Improve `List`
* [ ] Matrix support
* [ ] Matrix operations
* [ ] Better error handling
* [ ] Documentation
* [ ] Testing framework
* [ ] Stable public API
* [ ] First stable release

---

## ⚠️ Development Status

**Cobalt is currently experimental.**

The API and internal architecture are subject to change.

The project is primarily being developed as an exploration of generic programming, dynamic values, data structures, and abstraction techniques in C.

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

C is powerful precisely because it gives the programmer control.

But that control can also make higher-level abstractions tedious to implement.

Cobalt explores a middle ground:

```text
        High-level convenience
                 ▲
                 │
             Cobalt
                 │
                 ▼
        Low-level C control
```

**Less boilerplate. More capability. Still C.**
