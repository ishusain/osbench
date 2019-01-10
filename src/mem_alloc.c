// This is free and unencumbered software released into the public domain.
// For more information, see UNLICENSE.

#include "common/time.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define ITERS 1000
#define FOUR_K 0x1000


int main(int argc, const char** argv) {
  (void)argc;
  (void)argv;

  printf("Benchmark: Allocate/free %d memory chunks (4-128 bytes)...\n", ITERS);
  fflush(stdout);

  size_t size = 1;
  size_t counter = 0;
  int error = 0;


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

    char *mem = (char*) mmap(NULL, i * FOUR_K, PROT_READ | PROT_WRITE, MAP_ANON, -1, 0);
    if(mem)
    {
      // enforce memory allocation by writing to it
      for(unsigned j = 0; j < i; j++)
      {
        mem[j * FOUR_K] = 42;
      }
    }
    else
    {
      error = 1;
       break;
    }

    allocs[counter] = (void *) mem;

    counter += 1;
  }

  for (int i = 0; i < counter; ++i) {
    munmap(allocs[i], (i+1)*FOUR_K);
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

