// This is free and unencumbered software released into the public domain.
// For more information, see UNLICENSE.

#include "common/time.hpp"

#include <stdio.h>
#include <stdlib.h>


// Unix implementation.
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef pid_t process_t;

static process_t create_process(int process_no) {
  pid_t pid = fork();
  if (pid == 0) {
    execlp("/usr/bin/true", "true", (const char *) NULL);
  } else if (pid < 0) {
    fprintf(stderr, "*** Unable to create process no. %d\n", process_no);
    exit(1);
  }
  return (process_t)pid;
}

static void wait_process(process_t pid) {
  waitpid(pid, (int*)0, 0);
}


#define NUM_PROGRAMS 5000


int main(int argc, char** argv) {

  printf("Benchmark: Launch %d programs...\n", NUM_PROGRAMS);
  fflush(stdout);


  process_t processes[NUM_PROGRAMS];
  const double t0 = get_time();

  // Create all the processes.
  for (int i = 0; i < NUM_PROGRAMS; ++i) {
    processes[i] = create_process(i);
  }

  // Wait for all child processes to terminate.
  for (int i = 0; i < NUM_PROGRAMS; ++i) {
    wait_process(processes[i]);
  }

  double best_time = get_time() - t0;

  printf("%f us / program\n", (best_time / (double)NUM_PROGRAMS) * 1000000.0);
  fflush(stdout);

  return 0;
}

