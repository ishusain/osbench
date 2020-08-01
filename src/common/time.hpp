// This is free and unencumbered software released into the public domain.
// For more information, see UNLICENSE.

#ifndef COMMON_TIME_H_
#define COMMON_TIME_H_

#include <cstdio>
#include <iostream>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/time.h>
#endif

#if defined(_WIN32)
static LARGE_INTEGER s_start;
static double s_period = 0.0;
#endif

double get_time();
struct timeval get_timeval();
int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y);

#endif  // COMMON_TIME_H_
