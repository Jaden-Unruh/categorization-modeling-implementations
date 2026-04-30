#include <stdio.h>
#include "utils.h"
#include <time.h>

void printDoubleArray(double* arr, int n) {
	printf("[ ");

	for (int i = 0; i < n; i++)
		printf("%f, ", arr[i]);

	printf("]\n");
	return;
}

void printTime() {
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	printf("Time: %ld.%09ld\n", ts.tv_sec, ts.tv_nsec);
	return;
}
