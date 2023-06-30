//
// >>>> malloc challenge! <<<<
//
// Your task is to improve utilization and speed of the following malloc
// implementation.
// Initial implementation is the same as the one implemented in simple_malloc.c.
// For the detailed explanation, please refer to simple_malloc.c.

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//
// Interfaces to get memory pages from OS
//

void *mmap_from_system(size_t size);
void munmap_to_system(void *ptr, size_t size);

//
// Struct definitions
//

typedef struct my_metadata_t {
  size_t size;
  struct my_metadata_t *next;
} my_metadata_t;

typedef struct my_heap_t {
  my_metadata_t *free_head;
  my_metadata_t dummy;
} my_heap_t;

//
// Static variables (DO NOT ADD ANOTHER STATIC VARIABLES!)
//
my_heap_t my_heap[14];

//
// Helper functions (feel free to add/remove/edit!)
//

void my_add_to_free_list(my_metadata_t *metadata) {
  assert(!metadata->next);
  int size, bin_number;
  size = metadata->size;
  bin_number = size/1000;
  metadata->next = my_heap[bin_number].free_head;
  my_heap[bin_number].free_head = metadata;
}

void my_remove_from_free_list(my_metadata_t *metadata, my_metadata_t *prev) {
  int size, bin_number;
  size = metadata->size;
  bin_number = size/1000;
  if (prev) {
    prev->next = metadata->next;
  } else {
    my_heap[bin_number].free_head = metadata->next;
  }
  metadata->next = NULL;
}

//
// Interfaces of malloc (DO NOT RENAME FOLLOWING FUNCTIONS!)
//

// This is called at the beginning of each challenge.
void my_initialize() {
  for(int i=0; i<14; i++){
    my_heap[i].free_head = &my_heap[i].dummy;
    my_heap[i].dummy.size = 0;
    my_heap[i].dummy.next = NULL;
  }
}

// my_malloc() is called every time an object is allocated.
// |size| is guaranteed to be a multiple of 8 bytes and meets 8 <= |size| <=
// 4000. You are not allowed to use any library functions other than
// mmap_from_system() / munmap_to_system().
void *my_malloc(size_t size) {
  // First-fit: Find the first free slot the object fits.
  // TODO: Update this logic to Best-fit!
  
  
  // program for free list bin
  int bin_number;
  bin_number = size/1000;
  my_metadata_t *metadata = my_heap[bin_number].free_head;
  my_metadata_t *prev = NULL;
  
  my_metadata_t *suitable = NULL;
  my_metadata_t *prev_suitable = NULL;
  double used;
  
  //find first metadata
 while(bin_number<14){
     metadata = my_heap[bin_number].free_head;
     prev = NULL;
     while (metadata && metadata->size < size) {
         prev = metadata;
         metadata = metadata->next;
      }
      if (metadata){
          prev_suitable = prev;
          suitable = metadata;
          used =  size * 1.0 / suitable->size;
          break;
        }
        else{
            bin_number += 1;
        }
  }
  
  // improve metadata
    while (metadata) {
        if((metadata->size > size) && ((size * 1.0 / metadata->size )>used)){
            suitable = metadata;
            prev_suitable = prev;
            used = size * 1.0 /suitable->size;
            }
        prev = metadata;
        metadata = metadata->next;
        if(used > 0.9){
          break;
        }
    }
        metadata = suitable;
        prev = prev_suitable;
  
  // program for best_fit
  /*my_metadata_t *best_data = NULL;
  my_metadata_t *prev_best_data = NULL;
  int waste = INT_MAX;
  while (metadata && metadata->size < size) {
    prev = metadata;
    metadata = metadata->next;
  }
  best_data = metadata;
  prev_best_data = prev;
  if (metadata){
      waste = metadata->size - size;
    }
  while (metadata) {
    if((metadata->size > size) && ((metadata->size - size)<waste)){
        best_data = metadata;
        prev_best_data = prev;
        waste = best_data->size - size;
    }
    prev = metadata;
    metadata = metadata->next;
  }
  metadata = best_data;
  prev = prev_best_data;*/
  
  
  // program for worst_fit
  /*my_metadata_t *worst_data = NULL;
  my_metadata_t *prev_worst_data = NULL;
  int memory_size = 0;
  while (metadata && metadata->size < size) {
    prev = metadata;
    metadata = metadata->next;
  }
  worst_data = metadata;
  prev_worst_data = prev;
  if (metadata){
      memory_size = metadata->size;
    }
  while (metadata) {
    if((metadata->size > size) && (metadata->size > memory_size)){
        worst_data = metadata;
        prev_worst_data = prev;
        memory_size = worst_data->size;
    }
    prev = metadata;
    metadata = metadata->next;
  }
  metadata = worst_data;
  prev = prev_worst_data;*/
  
  
  // now, metadata points to the first free slot
  // and prev is the previous entry.

  if (!metadata) {
    // There was no free slot available. We need to request a new memory region
    // from the system by calling mmap_from_system().
    //
    //     | metadata | free slot |
    //     ^
    //     metadata
    //     <---------------------->
    //            buffer_size
    size_t buffer_size = 4096;
    my_metadata_t *metadata = (my_metadata_t *)mmap_from_system(buffer_size);
    metadata->size = buffer_size - sizeof(my_metadata_t);
    metadata->next = NULL;
    // Add the memory region to the free list.
    my_add_to_free_list(metadata);
    // Now, try my_malloc() again. This should succeed.
    return my_malloc(size);
  }

  // |ptr| is the beginning of the allocated object.
  //
  // ... | metadata | object | ...
  //     ^          ^
  //     metadata   ptr
  void *ptr = metadata + 1;
  size_t remaining_size = metadata->size - size;
  //metadata->size = size;
  // Remove the free slot from the free list.
  my_remove_from_free_list(metadata, prev);

  if (remaining_size > sizeof(my_metadata_t)) {
    // Create a new metadata for the remaining free slot.
    //
    // ... | metadata | object | metadata | free slot | ...
    //     ^          ^        ^
    //     metadata   ptr      new_metadata
    //                 <------><---------------------->
    //                   size       remaining size
    // Shrink the metadata for the allocated object
    // to separate the rest of the region corresponding to remaining_size.
    // If the remaining_size is not large enough to make a new metadata,
    // this code path will not be taken and the region will be managed
    // as a part of the allocated object.
    metadata->size = size;
    my_metadata_t *new_metadata = (my_metadata_t *)((char *)ptr + size);
    new_metadata->size = remaining_size - sizeof(my_metadata_t);
    new_metadata->next = NULL;
    // Add the remaining free slot to the free list.
    my_add_to_free_list(new_metadata);
  }
  return ptr;
}

// This is called every time an object is freed.  You are not allowed to
// use any library functions other than mmap_from_system / munmap_to_system.
void my_free(void *ptr) {
  // Look up the metadata. The metadata is placed just prior to the object.
  //
  // ... | metadata | object | ...
  //     ^          ^
  //     metadata   ptr
  my_metadata_t *metadata = (my_metadata_t *)ptr - 1;
  // Add the free slot to the free list.
  my_add_to_free_list(metadata);
}

// This is called at the end of each challenge.
void my_finalize() {
  // Nothing is here for now.
  // feel free to add something if you want!
}

void test() {
  // Implement here!
  assert(1 == 1); /* 1 is 1. That's always true! (You can remove this.) */
}
