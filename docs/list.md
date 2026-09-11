# Cobalt Lists

`Clist` is Cobalt's dynamic, heterogeneous list type.

A Cobalt List stores references to `tPtr` objects rather than copying the `tPtr` structures themselves. The underlying memory is managed by **Tony**, Cobalt's memory-management system.

Lists can contain different data types and can contain other Lists, allowing arbitrarily nested structures.

---

## Overview

A `Clist` is defined as:

```c
typedef struct {
    size_t length;
    tPtr items;
} Clist;
```

The `items` field is a Tony-managed allocation containing an array of `tPtr *`.

Conceptually:

```text
Clist
├── length
└── items : tPtr
       │
       └── ptr → [ tPtr* ][ tPtr* ][ tPtr* ] ...
                       │
                       ├──→ tPtr → data
                       ├──→ tPtr → data
                       └──→ tPtr → data
```

This means the List stores **references** to `tPtr` objects.

---

## Memory Ownership

Lists do not own the allocations they reference.

Tony owns the underlying allocations.

For example:

```c
int value = 10;

Clist list = createList();

listAppend(&list, &value);
```

The List stores a reference to the `tPtr` created for the element. The List itself does not become responsible for destroying that allocation.

Similarly, removing an element from a List only removes its reference:

```c
listRemove(&list, 0);
```

It does **not** destroy the underlying Tony allocation.

Global cleanup is handled by:

```c
freeAll();
```

This separation allows multiple Lists to reference the same object.

---

# Creating a List

Use `createList()` to create an empty List:

```c
Clist list = createList();
```

A newly created List has:

```text
length = 0
items  = NULL
```

Example:

```c
Clist list = createList();

listPrint(&list);
```

Output:

```text
[]
```

---

# Appending Elements

Elements are added using:

```c
listAppend(&list, data);
```

The List accepts pointers to the data being stored.

For example:

```c
int value = 42;

listAppend(&list, &value);
```

Cobalt determines the type of the supplied pointer and stores the appropriate `DataType`.

### Strings

Cobalt uses `CString` for strings:

```c
CString name = createString("Sahil");

listAppend(&list, &name);
```

Strings are therefore explicitly represented as Cobalt strings rather than relying on raw `char *` type detection.

---

# Getting an Element

Use:

```c
tPtr listGet(Clist *list, size_t index);
```

Example:

```c
tPtr value = listGet(&list, 0);

print(value);
```

`listGet()` returns a `tPtr` by value.

If the List is `NULL`, empty, or the index is outside the List's bounds, an empty `tPtr` is returned.

For example:

```c
tPtr value = listGet(&list, 100);

print(value);
```

produces:

```text
<null>
```

---

# Removing an Element

Use:

```c
bool listRemove(Clist *list, size_t index);
```

`listRemove()` removes the reference at the specified index and shifts subsequent elements toward the beginning of the List.

Example:

```c
[10, 20, 30, 40]
```

After:

```c
listRemove(&list, 1);
```

the List becomes:

```text
[10, 30, 40]
```

Removing an invalid index returns `false`.

---

# Popping an Element

Use:

```c
tPtr listPop(Clist *list);
```

`listPop()` removes and returns the last element.

Example:

```c
[10, 20, 30]
```

```c
tPtr value = listPop(&list);
```

The List becomes:

```text
[10, 20]
```

and `value` contains the removed element.

Calling `listPop()` on an empty or invalid List returns an empty `tPtr`.

---

# Setting an Element

Use:

```c
bool listSet(Clist *list, size_t index, tPtr *pointer);
```

This replaces the `tPtr *` reference stored at the specified index.

The operation changes the List's reference; it does not destroy either the old or new allocation.

---

# Clearing a List

Use:

```c
bool listClear(Clist *list);
```

`listClear()` removes all logical elements from the List.

Example:

```c
[10, 20, 30]
```

After:

```c
listClear(&list);
```

the List becomes:

```text
[]
```

The List can then be reused:

```c
int value = 999;

listAppend(&list, &value);
```

resulting in:

```text
[999]
```

`listClear()` does not destroy the underlying Tony allocations referenced by the List.

---

# Swapping Elements

Use:

```c
bool listSwap(
    Clist *list,
    size_t index_to_swap,
    size_t index_value_toswap
);
```

This exchanges two references in the List.

Example:

```text
[10, 20, 30]
```

```c
listSwap(&list, 0, 2);
```

becomes:

```text
[30, 20, 10]
```

Swapping an index outside the List's bounds returns `false`.

Swapping an element with itself is valid.

---

# Reversing a List

Use:

```c
void listReverse(Clist *list);
```

`listReverse()` reverses the order of the references in the List.

Example:

```text
[1, 2, 3, 4, 5]
```

After:

```c
listReverse(&list);
```

the List becomes:

```text
[5, 4, 3, 2, 1]
```

The implementation uses `listSwap()` to exchange elements from opposite ends of the List.

Reversing an empty or single-element List has no effect.

---

# Nested Lists

Lists can contain other Lists.

Example:

```c
Clist parent = createList();
Clist child = createList();

int value = 10;

listAppend(&child, &value);
listAppend(&parent, &child);

listPrint(&parent);
```

Output:

```text
[[10]]
```

Nested Lists can be arbitrarily deep.

For example:

```c
Clist l1 = createList();
Clist l2 = createList();
Clist l3 = createList();
Clist l4 = createList();

int value = 12345;

listAppend(&l4, &value);
listAppend(&l3, &l4);
listAppend(&l2, &l3);
listAppend(&l1, &l2);

listPrint(&l1);
```

Output:

```text
[[[[12345]]]]
```

Nested Lists are references, so changes made to a referenced child List are visible through its parent.

---

# Shared References

Because Lists store references, multiple List elements can reference the same object.

For example:

```c
Clist list = createList();

int value = 777;

listAppend(&list, &value);
listAppend(&list, &value);
listAppend(&list, &value);
```

produces:

```text
[777, 777, 777]
```

Similarly, the same nested List can be referenced multiple times:

```c
Clist child = createList();
Clist parent = createList();

int value = 555;

listAppend(&child, &value);

listAppend(&parent, &child);
listAppend(&parent, &child);
listAppend(&parent, &child);
```

Output:

```text
[[555], [555], [555]]
```

If the child is subsequently modified:

```c
int another = 999;

listAppend(&child, &another);
```

the parent reflects the change:

```text
[[555, 999], [555, 999], [555, 999]]
```

---

# Mixed Types

A List can contain different Cobalt types simultaneously.

For example:

```c
Clist list = createList();

int integer = 42;
float floating = 3.14f;
double decimal = 123456.789;

CString string = createString("Cobalt");

listAppend(&list, &integer);
listAppend(&list, &floating);
listAppend(&list, &decimal);
listAppend(&list, &string);
```

Output:

```text
[42, 3.140000, 123456.789000, "Cobalt"]
```

Lists can also mix nested Lists with other types.

Example:

```text
[10, [3.14, "Sahil", ['X', [42, "Cobalt", 2.718000], "Hello"], 99.123456], "Cobalt"]
```

---

# Printing Lists

Use:

```c
listPrint(&list);
```

`listPrint()` recursively prints nested Lists.

Example:

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

Nested Lists are automatically expanded:

```text
[[10, "Sahil"], [20, "Cobalt"]]
```

The public `listPrint()` function terminates its output with a newline. Recursive List printing does not introduce additional newlines between nested Lists.

---

# API Reference

## `createList`

```c
Clist createList(void);
```

Creates and returns an empty List.

---

## `listAppend`

```c
bool listAppend(Clist *list, void *data);
```

Adds an element to the end of a List.

Returns:

* `true` on success
* `false` if the List is invalid or an allocation fails

---

## `listGet`

```c
tPtr listGet(Clist *list, size_t index);
```

Returns the element at `index`.

Returns an empty `tPtr` for an invalid List or index.

---

## `listRemove`

```c
bool listRemove(Clist *list, size_t index);
```

Removes the reference at `index`.

Returns:

* `true` on success
* `false` for an invalid List or index

---

## `listPop`

```c
tPtr listPop(Clist *list);
```

Removes and returns the final element.

Returns an empty `tPtr` if no element can be popped.

---

## `listSet`

```c
bool listSet(Clist *list, size_t index, tPtr *pointer);
```

Replaces the reference stored at `index`.

---

## `listClear`

```c
bool listClear(Clist *list);
```

Removes all logical elements from the List.

---

## `listSwap`

```c
bool listSwap(
    Clist *list,
    size_t index_to_swap,
    size_t index_value_toswap
);
```

Swaps two elements.

---

## `listReverse`

```c
void listReverse(Clist *list);
```

Reverses the List.

---

## `listPrint`

```c
void listPrint(Clist *list);
```

Prints the List recursively.

---

# Edge Cases

The List implementation handles invalid and boundary operations without crashing.

Examples include:

```c
Clist *list = NULL;

listGet(list, 0);
listRemove(list, 0);
listPop(list);
listClear(list);
listSwap(list, 0, 0);
listReverse(list);
```

Invalid indexes are also rejected:

```c
listGet(&list, list.length);
listRemove(&list, list.length);
listSwap(&list, 0, list.length);
```

An empty List remains valid after operations such as:

```c
listReverse(&list);
listClear(&list);
```

A List can also be reused after being cleared.

---

# Example

A complete example combining several features:

```c
#include <cobalt/cobalt.h>

int main(void)
{
    Clist list = createList();
    Clist nested = createList();

    int value = 10;
    float pi = 3.14f;

    CString name = createString("Sahil");

    listAppend(&nested, &value);
    listAppend(&nested, &name);

    listAppend(&list, &pi);
    listAppend(&list, &nested);

    listPrint(&list);

    freeAll();

    return 0;
}
```

Output:

```text
[3.140000, [10, "Sahil"]]
```

---

# Design Summary

Cobalt Lists intentionally keep their responsibilities small:

```text
Tony
 └── owns allocations

tPtr
 └── describes a Tony allocation

CString
 └── provides the Cobalt string abstraction

Clist
 └── stores and orders references to tPtr objects
```

The List does not become an ownership or memory-management system of its own.

This makes nested Lists, shared references, mixed types, and List operations possible without duplicating Tony's memory-management responsibilities.
