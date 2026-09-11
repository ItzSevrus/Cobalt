# CString

CString is Cobalt's string abstraction. It wraps a null-terminated character buffer managed by Tony and stores the string length explicitly.

## Current implementation

```c
typedef struct {
    tPtr pointer;
    size_t length;
} CString;
```

A `CString` therefore contains:

- `pointer` — a `tPtr` describing the Tony-managed character buffer.
- `length` — the number of characters in the string, excluding the terminating `'\0'`.

For example:

```text
CString
├── pointer
│   ├── ptr  → "Hello\0"
│   └── type → TYPE_STRING
└── length → 5
```

The character buffer is null-terminated, so it can still be used as a normal C string internally.

---

## Including CString

CString is part of the public Cobalt API.

```c
#include <cobalt/Cstring.h>
```

The exact public include path should follow the Cobalt include layout used by the project.

---

# Creating Strings

## `createString()`

```c
CString createString(const char *string);
```

Creates a CString from a normal C string.

Current implementation:

```c
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
```

### Example

```c
CString name = createString("Sahil");
```

The resulting object contains:

```text
length = 5
buffer = "Sahil\0"
type   = TYPE_STRING
```

The terminating null byte is allocated but is not included in `length`.

Therefore:

```text
"Cobalt"
length = 6
allocated bytes = 7
```

## Empty strings

An empty C string is valid:

```c
CString empty = createString("");
```

Its state is:

```text
length = 0
buffer = "\0"
```

This is different from an invalid/uninitialized CString:

```c
CString invalid = {0};
```

An invalid CString has no character buffer.

---

# String Length

## `stringLength()`

```c
size_t stringLength(CString *string);
```

Returns the stored length of a CString.

```c
CString text = createString("Hello");

printf("%zu\n", stringLength(&text));
```

Output:

```text
5
```

The function uses the stored `length` field instead of repeatedly calling `strlen()`.

For a `NULL` CString pointer or a CString with no allocated buffer:

```c
stringLength(NULL);
```

returns:

```text
0
```

---

# String Equality

## `stringEquals()`

```c
bool stringEquals(CString *a, CString *b);
```

Checks whether two valid CStrings contain the same sequence of characters.

Example:

```c
CString a = createString("Cobalt");
CString b = createString("Cobalt");
CString c = createString("Tony");

printf("%s\n",
       stringEquals(&a, &b) ? "true" : "false");

printf("%s\n",
       stringEquals(&a, &c) ? "true" : "false");
```

Output:

```text
true
false
```

The implementation first compares lengths. If the lengths differ, the strings cannot be equal. If they match, the character buffers are compared with `memcmp()`.

Equality is case-sensitive:

```text
"Cobalt" != "cobalt"
```

Invalid/NULL string objects return `false`.

---

# Searching Inside Strings

## `stringContains()`

```c
bool stringContains(CString *string, CString *substring);
```

Checks whether `substring` occurs inside `string`.

Example:

```c
CString text = createString(
    "Cobalt is a high level C library"
);

CString a = createString("high level");
CString b = createString("Python");

printf("%s\n",
       stringContains(&text, &a) ? "true" : "false");

printf("%s\n",
       stringContains(&text, &b) ? "true" : "false");
```

Output:

```text
true
false
```

The current implementation performs a straightforward byte comparison using `memcmp()`.

An empty substring is considered contained:

```c
CString empty = createString("");

stringContains(&text, &empty);
```

returns:

```text
true
```

If the substring is longer than the source string, the function returns `false`.

The comparison is case-sensitive.

---

# Trimming Strings

## `stringTrim()`

```c
CString stringTrim(CString *string);
```

Creates a new CString with whitespace removed from both ends.

Example:

```c
CString text = createString(
    "   Hello Cobalt   "
);

CString trimmed = stringTrim(&text);
```

Result:

```text
"Hello Cobalt"
```

The current implementation uses `isspace()` and therefore trims C's recognized whitespace characters at the beginning and end of the string.

The original CString is not modified.

```text
original
"   Hello Cobalt   "

        │
        ▼

stringTrim()

        │
        ▼

new CString
"Hello Cobalt"
```

The returned CString has its own Tony-managed character buffer.

For a string containing only whitespace, the result is a valid empty CString:

```text
length = 0
buffer = "\0"
```

For an invalid/NULL input, a zero-initialized CString is returned.

---

# Splitting Strings

Cobalt currently provides two split functions:

```c
Clist stringSplit(CString *string);

Clist stringSplitBy(CString *string, const char *delimiter);
```

The split functions return a `Clist`.

Each resulting element is a CString stored in Tony-managed memory so that the elements remain valid after `stringSplitBy()` returns.

---

# `stringSplit()`

```c
Clist stringSplit(CString *string);
```

The default delimiter is a single space:

```text
" "
```

Example:

```c
CString text = createString(
    "Cobalt makes C easier"
);

Clist words = stringSplit(&text);

listPrint(&words);
```

Output:

```text
["Cobalt", "makes", "C", "easier"]
```

The current implementation is:

```c
Clist stringSplit(CString *string)
{
    return stringSplitBy(string, " ");
}
```

Therefore `stringSplit()` is a convenience wrapper around `stringSplitBy()`.

## Consecutive spaces

The current implementation treats every delimiter occurrence as a separator.

For:

```text
"one  two   three"
```

the result is:

```text
["one", "", "two", "", "", "three"]
```

This behavior is intentional in the current implementation: consecutive delimiters produce empty fields.

---

# `stringSplitBy()`

```c
Clist stringSplitBy(CString *string, const char *delimiter);
```

Splits a CString using a user-provided delimiter.

Example:

```c
CString text = createString(
    "Sahil,20,Delhi,Cobalt"
);

Clist parts = stringSplitBy(&text, ",");

listPrint(&parts);
```

Output:

```text
["Sahil", "20", "Delhi", "Cobalt"]
```

The delimiter can contain more than one character.

Example:

```c
CString text = createString(
    "one::two::three::four"
);

Clist parts = stringSplitBy(&text, "::");

listPrint(&parts);
```

Output:

```text
["one", "two", "three", "four"]
```

Other possible delimiters include:

```c
stringSplitBy(&text, "|");
stringSplitBy(&text, "->");
stringSplitBy(&text, "||");
stringSplitBy(&text, "::");
```

---

# Empty Fields

`stringSplitBy()` preserves empty fields.

Example:

```c
CString text = createString(
    "one,,three,"
);

Clist result = stringSplitBy(&text, ",");

listPrint(&result);
```

Output:

```text
["one", "", "three", ""]
```

This behavior is important for future structured-data support, especially CSV, where an empty field can carry meaning.

---

# Invalid Split Input

The current implementation safely handles:

```c
stringSplit(NULL);
```

and returns an empty list:

```text
[]
```

`stringSplitBy()` returns an empty list when:

- `string == NULL`
- the CString has no character buffer
- `delimiter == NULL`
- the delimiter is an empty string

For example:

```c
CString invalid = {0};

Clist result = stringSplit(&invalid);
```

produces an empty list.

---

# Memory Model

CString uses Tony for its character storage.

For a normal CString:

```text
CString
   │
   └── tPtr
        │
        └── ptr → character buffer
```

For a CString stored inside a Clist:

```text
Clist
  │
  └── tPtr*
       │
       └── tPtr
            │
            └── ptr → CString
                         │
                         └── tPtr
                              │
                              └── ptr → character buffer
```

This distinction is important.

A standalone CString can be created normally:

```c
CString name = createString("Sahil");

listAppend(&list, &name);
```

The CString object itself is a normal local object, while its character buffer is managed by Tony.

Strings generated dynamically by `stringSplitBy()` require the CString object itself to have a lifetime beyond the function call. The current implementation therefore allocates each split CString object through Tony before placing it in the result Clist.

This prevents the list from storing pointers to temporary stack variables.

---

# Relationship With Clist

CString and Clist are designed to work together.

A string can be split into a list:

```text
CString
"Sahil loves Cobalt"
        │
        ▼
stringSplit()
        │
        ▼
Clist
["Sahil", "loves", "Cobalt"]
```

The resulting list contains CString objects.

This gives Cobalt a basic text-processing pipeline:

```text
CString
   ↓
split
   ↓
Clist
   ↓
CString elements
```

This is also useful as a foundation for future file and CSV functionality.

---

# Relationship With Tony

CString does not directly use `malloc()` for its character storage.

Instead:

```c
result.pointer = talloc(size);
```

is used.

This means CString participates in Cobalt's central memory-management system.

The character buffer is registered in Tony's memory pool and receives a `tPtr`.

For generated split strings, both the CString object and its character buffer are Tony-managed.

This keeps the lifetime of dynamically generated string objects compatible with Cobalt's current Clist reference model.

---

# Current Public API

The current CString API is:

```c
CString createString(const char *string);

size_t stringLength(CString *string);

bool stringEquals(CString *a, CString *b);

bool stringContains(CString *string, CString *substring);

CString stringTrim(CString *string);

Clist stringSplit(CString *string);

Clist stringSplitBy(CString *string, const char *delimiter);
```

---

# Complete Example

```c
#include <stdio.h>

#include <cobalt/Cstring.h>
#include <cobalt/Clist.h>

int main(void)
{
    CString text = createString(
        "   Cobalt makes C easier   "
    );

    printf("Original: \"");
    printString(&text);
    printf("\"\n");

    printf("Length: %zu\n", stringLength(&text));

    CString trimmed = stringTrim(&text);

    printf("Trimmed: \"");
    printString(&trimmed);
    printf("\"\n");

    Clist words = stringSplit(&trimmed);

    printf("Words: ");
    listPrint(&words);

    return 0;
}
```

Expected output:

```text
Original: "   Cobalt makes C easier   "
Length: 28
Trimmed: "Cobalt makes C easier"
Words: ["Cobalt", "makes", "C", "easier"]
```

---

# Current Design Principles

CString currently follows several simple principles:

### 1. Explicit length

The length is stored in the object:

```c
size_t length;
```

rather than recalculated whenever it is needed.

### 2. Null termination

The underlying buffer remains a normal C string:

```text
characters + '\0'
```

This keeps interoperability with existing C string functions possible.

### 3. Tony-managed memory

Character buffers are allocated through Cobalt's Tony memory system.

### 4. Immutable-style transformation functions

Functions such as:

```c
stringTrim()
stringSplit()
stringSplitBy()
```

do not modify the original CString. They create resulting objects/lists.

### 5. Clist integration

String operations can naturally produce Cobalt collections.

### 6. Simple C API

CString does not attempt to hide C completely. It provides a higher-level interface while retaining a straightforward C data model.

---

# Current Limitations

The current CString implementation is intentionally small.

The following functionality is **not currently part of the implemented API**:

```text
replace
find/index
startsWith
endsWith
join
case conversion
formatting
character insertion/removal
substring/slicing
```

These can be added as the String layer evolves.

CSV parsing is also **not part of CString**. Generic delimiter splitting is provided by:

```c
stringSplitBy()
```

CSV-specific parsing should eventually live in the File/CSV/Dataset layer so that CSV quoting and escaping rules do not get mixed into generic string splitting.

---

# Summary

CString provides Cobalt with a small but useful string abstraction:

```text
             CString
                │
       ┌────────┴────────┐
       │                 │
     length           tPtr
                         │
                         ▼
                   char buffer
```

Current capabilities:

```text
createString()
      │
      ├── stringLength()
      ├── stringEquals()
      ├── stringContains()
      ├── stringTrim()
      ├── stringSplit()
      └── stringSplitBy()
                         │
                         ▼
                       Clist
```

The current implementation is deliberately foundational. It provides the string functionality needed to build more capable Cobalt abstractions above it, particularly file processing, CSV parsing, and eventually datasets.
