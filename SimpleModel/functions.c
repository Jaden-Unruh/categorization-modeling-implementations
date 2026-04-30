#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
#include "utils.h"

double* normalize(double* nums, int n) {
	double total = 0;

	for (int i = 0; i < n; i++)
		total += nums[i];

	double per = 1 / total;

	for (int i = 0; i < n; i++)
		nums[i] *= per;

	return nums;
}

double* resetWeights(char* QMfile, int length) {
	FILE* file = fopen(QMfile, "r");
	if (!file) {
		perror("Failed to open file");
		return NULL;
	}

	double* arr = malloc(length * sizeof(double));
	if (!arr) {
		perror("Failed to allocate memory");
		fclose(file);
		return NULL;
	}

	size_t i = 0;
	while (i < length && fscanf(file, "%lf", &arr[i]) == 1) {
		i++;
	}

	fclose(file);

	if (i != length) {
		fprintf(stderr, "Expected %zu values, got %zu\n", length, i);
		free(arr);
		return NULL;
	}

	return arr;
}

double* updateWeights(double* weights, bool* traits, bool isEvil, int length, double CONFIDENCE_FACTOR) {

	for (int i = 0; i < length; i++)
		weights[i] *= isCorrect(i, traits, isEvil) ? 1 : CONFIDENCE_FACTOR;

	normalize(weights, length);

	return weights;
}

bool isCorrect(int row, bool* traits, bool isEvil) {
	bool prediction = (row >> boolArray4ToInt(traits)) & 1;
	return prediction == isEvil;
}

int boolArray4ToInt(bool* arr) {
	int result = 0;
	for (int i = 0; i < 4; i++)
		result = (result << 1 | arr[i]);
	return result;
}

double getCertainty(double* weights, bool* traits, int length) {
	double certainty = 0;
	int index = boolArray4ToInt(traits);
	for (int i = 0; i < length; i++)
		certainty += ((i >> index) & 1) ? weights[i] : 0;

	return certainty;
}
