// This is free and unencumbered software released into the public domain.
// For more information, see UNLICENSE.

#include "common/time.hpp"

double get_time()
{
	double result;
#if defined(_WIN32)
	LARGE_INTEGER count;
	if (s_period == 0.0)
	{
		LARGE_INTEGER frequency;
		if (QueryPerformanceFrequency(&frequency))
		{
			s_period = 1.0 / (double)frequency.QuadPart;
			QueryPerformanceCounter(&s_start);
		}
	}
	if (QueryPerformanceCounter(&count))
	{
		result = (count.QuadPart - s_start.QuadPart) * s_period;
	}
	else
	{
		result = 0.0;
	}
#else
	struct timeval tv;
	if (gettimeofday(&tv, (struct timezone *)0) == 0)
	{
		result = ((double)tv.tv_sec) + 1e-6 * (double)tv.tv_usec;
	}
	else
	{
		result = 0.0;
	}
#endif
	return result;
}

struct timeval get_timeval()
{
	double result;
#if defined(_WIN32)
	LARGE_INTEGER count;
	if (s_period == 0.0)
	{
		LARGE_INTEGER frequency;
		if (QueryPerformanceFrequency(&frequency))
		{
			s_period = 1.0 / (double)frequency.QuadPart;
			QueryPerformanceCounter(&s_start);
		}
	}
	if (QueryPerformanceCounter(&count))
	{
		result = (count.QuadPart - s_start.QuadPart) * s_period;
	}
	else
	{
		result = 0.0;
	}
#else
	struct timeval tv;
	if (gettimeofday(&tv, (struct timezone *)0) == 0)
	{
		return tv;
	}
	else
	{
		printf("Invalid timeval struct.\n");
	}
#endif
	return tv;
}

/* Subtract the `struct timeval' values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0.  */

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_usec < y->tv_usec)
	{
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000)
	{
		int nsec = (x->tv_usec - y->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}

	/* Compute the time remaining to wait.
	   tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;

	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}