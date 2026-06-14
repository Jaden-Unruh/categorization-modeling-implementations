#include <stdbool.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

int choose(int n, int k);
void reset(double *weights, int length);
void normalize(double *weights, int length);
void generate_combinations(bool *combinations, int n, int c);
void generate_padded_representations(char *padded_representations, int considered, int groups, int selection_length);
int index_from_all_traits(bool *traits, int selection, bool *combinations, int num_vars);
int get_max_index(double *array, int length);

#endif