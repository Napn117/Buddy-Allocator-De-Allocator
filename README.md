# Buddy System Memory Allocator – C++ Implementation

## 🧠 Overview
This project implements a **Buddy System memory allocator** in C++, replicating the behavior of the standard `malloc()` and `free()` functions. It manages a contiguous memory block by splitting and coalescing memory into powers of two, improving allocation efficiency and reducing fragmentation.

The allocator is tested against randomized memory allocation and deallocation patterns, and is compared to standard C memory allocation in terms of **runtime** and **memory usage**.

---

## 🔨 Implementation Details

### ✔️ Core Functions
- **`void* buddyMalloc(int size)`**  
  Allocates memory from the managed block using the buddy allocation strategy.

- **`int buddyFree(void* ptr)`**  
  Frees previously allocated memory and merges buddy blocks when possible.

These functions are implemented in `buddysys.cpp` and follow the same interface as `malloc` and `free`.

### ✔️ Supporting Infrastructure
- **Block metadata** is tracked using a custom `Node` structure.
- **Free lists** are organized by block size (powers of 2), allowing fast lookup and merging.
- **Splitting and coalescing** logic ensures minimal fragmentation.
- The implementation avoids all built-in allocators (`malloc`, `new`, etc.).

---

# ⚙️ How to Build & Run

```bash
make
main.exe
