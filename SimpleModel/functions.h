#ifndef FUNCTIONS_H
#define FUNCTIONS_H

double* normalize(double* nums, int n);
double* resetWeights(char* QMfile, int length);
double* updateWeights(double* weights, bool* traits, bool isEvil, int length, double CONFIDENCE_FACTOR);
bool isCorrect(int row, bool* traits, bool isEvil);
int boolArray4ToInt(bool* arr);
double getCertainty(double* weights, bool* traits, int length);

#endif
