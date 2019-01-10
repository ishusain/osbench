// This is free and unencumbered software released into the public domain.
// For more information, see UNLICENSE.

#include "common/time.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static const int NUM_PROCESSES = 5000;

int main() {
  printf("Benchmark: Create/teardown of %d processes...\n", NUM_PROCESSES);
  fflush(stdout);

  pid_t processes[NUM_PROCESSES];
  const double t0 = get_time();

  // Create all the processes.
  for (int i = 0; i < NUM_PROCESSES; ++i) {
    pid_t pid = fork();
    if (pid == 0) {
      exit(0);
    } else if (pid > 0) {
      processes[i] = pid;
    } else {
      fprintf(stderr, "*** Unable to create process no. %d\n", i);
      exit(1);
    }
  }

  // Wait for all child processes to terminate.
  for (int i = 0; i < NUM_PROCESSES; ++i) {
    waitpid(processes[i], (int*)0, 0);
  }

  double best_time = get_time() - t0;

  printf("%f us / process\n", (best_time / (double)NUM_PROCESSES) * 1000000.0);
  fflush(stdout);

  return 0;
}

