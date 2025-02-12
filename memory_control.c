#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Define memory pool size and magic number for checksum verification
#define MEMORY_POOL_SIZE 4096
#define MAGIC_NUMBER 0xDEADBEEF

// Structure to represent a free memory block
typedef struct Free_memory_block {
    size_t virtual_addr; // Address within the memory pool
    size_t block_size;   // Size of the free block
    struct Free_memory_block* next; // Pointer to the next free block
} Free_memory_block;

// Structure to represent an allocated memory block header
typedef struct header {
    size_t checksum;      // Checksum for integrity verification
    size_t allocated_size; // Size of allocated memory (excluding header)
    size_t virtual_addr;  // Virtual address within the memory pool
} header;

// Global pointer to the free memory list
Free_memory_block* free_list = NULL;

// Initializes the memory pool using mmap
void init_memory_pool() {
  free_list = mmap(NULL, MEMORY_POOL_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
  if (free_list == MAP_FAILED) {
    perror("mmap failed");
    return;
  }
  // Initialize the free list with the entire pool
  free_list->virtual_addr = 0;
  free_list->block_size = MEMORY_POOL_SIZE - sizeof(Free_memory_block);
  free_list->next = NULL;
}

// Calculates a simple checksum for validation
size_t calculate_checksum(header* hdr) {
  return hdr->allocated_size ^ hdr->virtual_addr ^ MAGIC_NUMBER;
}

// Prints the current state of the free memory list
void print_free_list() {
  Free_memory_block* current = free_list;
  printf("Free list: head");
  while (current != NULL) {
    printf(" -> [addr:%zu, len:%zu]", current->virtual_addr, current->block_size);
    current = current->next;
  }
  printf(" -> NULL\n");
}

// Returns the pointer to the free memory list
Free_memory_block* get_free_list() {
  return free_list;
}

// Frees an allocated memory block and merges it back into the free list
int _free(void* ptr) {
  if (!ptr) return -1; // Null pointer check

  // Retrieve the header and validate checksum
  header* hdr_ptr = ((header*)ptr) - 1;
  if (hdr_ptr->checksum != calculate_checksum(hdr_ptr)) {
    fprintf(stderr, "Error: Invalid or corrupted block\n");
    return -1;
  }

  // Convert the used block into a free block
  Free_memory_block* free_block = (Free_memory_block*)hdr_ptr;
  free_block->virtual_addr = hdr_ptr->virtual_addr;
  free_block->block_size = hdr_ptr->allocated_size;

  // Insert the block back into the free list in sorted order
  Free_memory_block* cur_block = free_list;
  Free_memory_block* prev_block = NULL;
  while (cur_block && free_block->virtual_addr > cur_block->virtual_addr) {
    prev_block = cur_block;
    cur_block = cur_block->next;
  }

  free_block->next = cur_block;
  if (prev_block) {
    prev_block->next = free_block;
  } else {
    free_list = free_block; // Update head if freeing the smallest block
  }

  // Merge adjacent free blocks
  if (cur_block && free_block->virtual_addr + free_block->block_size + sizeof(Free_memory_block) == cur_block->virtual_addr) {
    free_block->block_size += cur_block->block_size + sizeof(Free_memory_block);
    free_block->next = cur_block->next;
  }
  if (prev_block && prev_block->virtual_addr + prev_block->block_size + sizeof(Free_memory_block) == free_block->virtual_addr) {
    prev_block->block_size += free_block->block_size + sizeof(Free_memory_block);
    prev_block->next = free_block->next;
  }

  return 0;
}

// Allocates memory from the free list
void* _malloc(size_t size) {
  Free_memory_block* cur_block = free_list;
  Free_memory_block* prev_block = NULL;
  size_t allocated_size = size + sizeof(header);

  while (cur_block) {
    if (cur_block->block_size >= allocated_size) {
      size_t remaining_size = cur_block->block_size - allocated_size;
      Free_memory_block* next_block = cur_block->next;
      header* hdr = (header*)cur_block; // Place the header at the start
      hdr->allocated_size = size;
      hdr->virtual_addr = cur_block->virtual_addr;
      hdr->checksum = calculate_checksum(hdr);

      if (remaining_size > sizeof(Free_memory_block)) {
        // Split the block
        Free_memory_block* new_block = (Free_memory_block*)((char*)hdr + allocated_size);
        new_block->block_size = remaining_size;
        new_block->virtual_addr = hdr->virtual_addr + allocated_size;
        new_block->next = next_block;
        if (prev_block) {
          prev_block->next = new_block;
        } else {
          free_list = new_block; // Update head if splitting the first block
        }
      } else {
        // Use the entire block
        if (prev_block) {
          prev_block->next = cur_block->next;
        } else {
          free_list = cur_block->next; // Update head
        }
      }
      return (void*)((char*)hdr + sizeof(header)); // Return data segment
    }
    prev_block = cur_block;
    cur_block = cur_block->next;
  }
  return NULL; // No suitable block found
}

// Reallocates memory to a new size
void* _realloc(void* ptr, size_t new_size) {
  if (new_size == 0) {
    _free(ptr);
    return NULL;
  }
  if (ptr == NULL) {
    return _malloc(new_size);
  }
  header* old_hdr = (header*)ptr - 1;
  size_t old_size = old_hdr->allocated_size;
  if (new_size <= old_size) {
    return ptr;
  }
  void* new_ptr = _malloc(new_size);
  if (!new_ptr) {
    return NULL;
  }
  memcpy(new_ptr, ptr, old_size);
  _free(ptr);
  return new_ptr;
}

// Allocates memory and initializes it to zero
void* _calloc(size_t num, size_t size) {
  size_t allocation_len = num * size;
  char* ptr = _malloc(allocation_len);
  if (!ptr) {
    return NULL;
  }
  memset(ptr, 0, allocation_len); // Zero out memory
  return (void*)ptr;
}
