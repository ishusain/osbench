// This is free and unencumbered software released into the public domain.
// For more information, see UNLICENSE.

#include "common/time.h"

#include <stdlib.h>
#include <stdio.h>

#include <pthread.h>



// POSIX thread implementation.
typedef pthread_t thread_t;

static void* thread_fun(void* arg) {
  // We do nothing here...
  (void)arg;
  return (void*)0;
}

static thread_t create_thread() {
  thread_t result;
  if(pthread_create(&result, (const pthread_attr_t*)0, thread_fun, (void*)0))
  {
	  perror("fooo");
    exit(-1);
  }
  return result;
}

static void join_thread(thread_t thread) {
  pthread_join(thread, (void**)0);
}


#define NUM_THREADS 2000

int main() {
  printf("Benchmark: Create/teardown of %d threads...\n", NUM_THREADS);
  fflush(stdout);


  thread_t threads[NUM_THREADS];
  const double t0 = get_time();

  // Create all the child threads.
  for (int i = 0; i < NUM_THREADS; ++i) {
    threads[i] = create_thread();
  }

  // Wait for all the child threads to finish.
  for (int i = 0; i < NUM_THREADS; ++i) {
    join_thread(threads[i]);
  }

  double dt = get_time() - t0;

  printf("%f us / thread\n", (dt * 1000000/ NUM_THREADS));
  fflush(stdout);

  return 0;
}

