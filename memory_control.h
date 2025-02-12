#ifndef _MEMORY_CONTROL_H_
#define _MEMORY_CONTROL_H_

#include <stddef.h>

// Define the size of the memory pool
#define MEMORY_POOL_SIZE 4096

// Structure for a free memory block
typedef struct Free_memory_block {
    int virtual_addr;
    size_t block_size;
    struct Free_memory_block *next;
} Free_memory_block;

// Structure for a memory block header
typedef struct header {
    int checksum;
    int allocated_size;
    int virtual_addr;
} header;

// Function declarations
void init_memory_pool();
void* _malloc(size_t size);
int _free(void* ptr);
void* _realloc(void* ptr, size_t new_size);
void print_free_list();
void* _calloc(size_t num, size_t size);
Free_memory_block * get_free_list();
#endif // _MEMORY_CONTROL_H_