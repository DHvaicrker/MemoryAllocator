# 🧠 Simple Memory Allocator

Welcome to the Simple Memory Allocator project! This project implements a custom memory management system in C, providing functions for memory allocation, deallocation, and reallocation using a global memory pool. 

## 📋 Overview

This module provides custom memory management functions including:
- Initialization of the memory pool
- Allocation of memory blocks
- Deallocation of memory blocks
- Reallocation of memory blocks
- Printing the current state of the free list

### Prerequisites

- A C compiler (e.g., `gcc`)
- `make` (optional, for using the provided Makefile)

### Building the Project

To build the project, you can use the provided Makefile:

```sh
make
```

Alternatively, you can compile the source files manually:

```sh
gcc -o memory_allocator main.c memory_control.c
```

### Running the Example

After building the project, you can run the example program:

```sh
./memory_allocator
```

## 📂 Project Structure

- `main.c`: Contains the main function and example usage of the memory management functions.
- `memory_control.c`: Implements the memory management functions.
- `memory_control.h`: Declares the memory management functions and data structures.

## 🛠️ Functions

### `void init_memory_pool()`
- **Description**: Initializes the global memory pool and sets up the initial free list.
- **Parameters**: None
- **Returns**: None

### `void* _malloc(size_t size)`
- **Description**: Allocates a block of memory of the specified size.
- **Parameters**:
  - `size`: The size of the memory block to allocate in bytes.
- **Returns**: A pointer to the allocated memory block, or `NULL` if the allocation fails.

### `int _free(void* ptr)`
- **Description**: Frees a previously allocated memory block.
- **Parameters**:
  - `ptr`: Pointer to the memory block to be freed.
- **Returns**: `0` on success, `-1` if the pointer is invalid or the block is corrupted.

### `void* _realloc(void* ptr, size_t new_size)`
- **Description**: Changes the size of the memory block pointed to by `ptr` to `new_size` bytes.
- **Parameters**:
  - `ptr`: Pointer to the memory block to be resized. If `NULL`, the function behaves like `_malloc`.
  - `new_size`: New size of the memory block in bytes.
- **Returns**: A pointer to the reallocated memory, or `NULL` if the reallocation fails.

### `void* _calloc(size_t num, size_t size)`
- **Description**: Allocates memory for an array of `num` elements of `size` bytes each and initializes all bytes to zero.
- **Parameters**:
  - `num`: Number of elements to allocate.
  - `size`: Size of each element in bytes.
- **Returns**: A pointer to the allocated memory, or `NULL` if the allocation fails.

### `void print_free_list()`
- **Description**: Prints the current state of the free list for debugging purposes.
- **Parameters**: None
- **Returns**: None

### `Free_memory_block* get_free_list()`
- **Description**: Returns the head of the free list.
- **Parameters**: None
- **Returns**: A pointer to the head of the free list.


### `header`
- **Description**: Represents the header of an allocated memory block.
- **Members**:
  - `size_t checksum`: Checksum for validating the block.
  - `size_t allocated_size`: The size of the allocated block in bytes.
  - `size_t virtual_addr`: The virtual address of the block.
