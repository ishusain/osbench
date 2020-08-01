// This is free and unencumbered software released into the public domain.
// For more information, see UNLICENSE.

#include "common/time.hpp"

#include <pthread.h>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>

// POSIX thread implementation.
typedef pthread_t thread_t;

static void* thread_fun(void* arg)
{
	// We do nothing here...
	(void)arg;
	return (void*)0;
}

static thread_t create_thread()
{
	thread_t result;
	if (pthread_create(&result, (const pthread_attr_t*)0, thread_fun, (void*)0))
	{
		perror("pthread_create");
		exit(-1);
	}
	return result;
}

static void join_thread(thread_t thread)
{
	pthread_join(thread, (void**)0);
}

#define NUM_THREADS 2000
 static const double BENCHMARK_TIME = 5.0; 

void measure_method1(double* best_time)
{
	const double start_t = get_time();
	while (get_time() - start_t < BENCHMARK_TIME)
	{
		thread_t threads[NUM_THREADS];
		const double t0 = get_time();

		// Create all the child threads.
		for (int i = 0; i < NUM_THREADS; ++i)
		{
			threads[i] = create_thread();
		}

		// Wait for all the child threads to finish.
		for (int i = 0; i < NUM_THREADS; ++i)
		{
			join_thread(threads[i]);
		}

		double dt = get_time() - t0;
		if (dt < *best_time)
		{
			*best_time = dt;
		}
	}
}

void measure_method2(std::chrono::microseconds::rep* best_time)
{
	// TODO: we should use std::chrono::duration to set max value.
	*best_time = 1e6;

	const struct timeval epoch_tv = get_timeval();

	while (difftime(get_timeval().tv_sec, epoch_tv.tv_sec) < BENCHMARK_TIME)
	{
		thread_t threads[NUM_THREADS];
		auto t1 = std::chrono::high_resolution_clock::now();

		// Create all the child threads.
		for (int i = 0; i < NUM_THREADS; ++i)
		{
			threads[i] = create_thread();
		}
		auto t2 = std::chrono::high_resolution_clock::now();
		std::chrono::microseconds::rep dt = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
		if (dt < *best_time)
		{
			*best_time = dt;
		}

		// Wait for all the child threads to finish.
		for (int i = 0; i < NUM_THREADS; ++i)
		{
			join_thread(threads[i]);
		}
	}
}

int main(int argc, char** argv)
{
	printf("Benchmark: Create/teardown of %d threads...\n", NUM_THREADS);
	fflush(stdout);

	if (argc == 2)
	{
		std::chrono::microseconds::rep best_time;
		measure_method2(&best_time);
		// TODO: fix this cast.
		std::cout << best_time / (double)NUM_THREADS << " us / thread" << std::endl;
	}
	else
	{
		double best_time = 1e9;
		measure_method1(&best_time);
		printf("%f us / thread\n", (best_time / (double)NUM_THREADS) * 1e6);
	}

	fflush(stdout);

	return 0;
}

