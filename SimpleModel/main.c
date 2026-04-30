#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "functions.h"
#include "utils.h"

#define NUM_VARS 4
#define CONFIDENCE_FACTOR 0.1

void main() {
	int length = pow(2, pow(2, NUM_VARS));

	double *weights = (double*)calloc(length, sizeof(double));

	if (weights == NULL) {
		printf("allocation failed!");
		return;
	}

	weights = resetWeights("simple-4values.csv", length);

	normalize(weights, length);

	bool traitsTest1[] = { false, false, false, false };
	bool traitsTest2[] = { true, true, true, true };
	bool traitsTest3[] = { true, true, true, false };
	bool traitsTest4[] = { false, false, false, true };
	bool traitsTest5[] = { true, false, false, true };

	weights = updateWeights(weights, traitsTest1, false, length, CONFIDENCE_FACTOR);
	weights = updateWeights(weights, traitsTest4, false, length, CONFIDENCE_FACTOR);

	double output1 = getCertainty(weights, traitsTest1, length);
	double output2 = getCertainty(weights, traitsTest5, length);

	printf("certainty = %f\n", output1);
	printf("certainty = %f\n", output2);

	free(weights);

	return;
}
