// This is free and unencumbered software released into the public domain.
// For more information, see UNLICENSE.

#include "common/time.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "unistd.h"

#define ITERS 1000


int main(int argc, const char** argv) {
  (void)argc;
  (void)argv;

  printf("Benchmark: Allocate/free %d memory chunks (4-128 bytes)...\n", ITERS);
  fflush(stdout);

  size_t size = 1;
  size_t counter = 0;
  int error = 0;

  long page_size = sysconf(_SC_PAGE_SIZE);
  if(page_size == -1)
  {
        perror("sysconf");
        exit(-1);
  }
  //printf("page size: %ld\n", page_size);


  void **allocs = (void **) realloc(NULL, size * sizeof(void *));
  if(allocs == NULL)
  {
    fprintf(stderr, "out of memory");
    exit(-1);
  }

  double t0 = get_time();

  for (int i = 1; i <= ITERS; ++i) {

    if(counter == size)
    {
      size *= 2;
      void **n_allocs = (void **) realloc(allocs, size * sizeof(void *));
      if(n_allocs == NULL)
      {
	 error = 1;
	 break;
      }
      allocs = n_allocs;
    }

    char *mem = (char*) mmap(
        (void *) NULL
        , i * page_size 
        , PROT_READ | PROT_WRITE
    , MAP_PRIVATE | MAP_ANONYMOUS
        , -1
        , 0
    );
    if(mem == MAP_FAILED)
    {
        perror("mmap");
      error = 1;
       break;
    }
    else
    {
      // enforce memory allocation by writing to it
      for(unsigned j = 0; j < i; j++)
      {
        mem[j * page_size] = 42;
      }
    }

    allocs[counter] = (void *) mem;

    counter += 1;
  }

  for (int i = 0; i < counter; ++i) {
    munmap(allocs[i], (i+1)*page_size);
  }

  free(allocs);

  if(error)
  {
    fprintf(stderr, "out of memory");
    exit(-1);
  }

  double dt = get_time() - t0;

  printf("%f ns / alloc\n", (dt * 1000000000 / ITERS));
  fflush(stdout);

  return 0;
}

