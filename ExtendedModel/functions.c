#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int factorial(int n) {
    int factorial = 1;
    for (int i = 2; i <= n; i++)
        factorial *= i;
    return factorial;
}

int choose(int n, int k) {
    return factorial(n) / (factorial(k) * factorial(n - k));
}

void reset(double *weights, int length) {
    double val = 1.0 / (double) length;
    for (int i = 0; i < length; i++)
        weights[i] = val;
}

void normalize(double *weights, int length) {
    double sum;
    for (int i = 0; i < length; i++)
        sum += weights[i];
    double mult = 1 / sum;
    for (int i = 0; i < length; i++)
        weights[i] *= mult;
}

void backtrack(bool *combinations, bool *current, int start, int count, int k, int *index, int idxper) {
    if (count == k) {
        for (int i = 0; i < idxper; i++)
            combinations[(*index)++] = current[i];
        return;
    }

    for (int i = start; i < idxper; i++) {
        current[i] = true;
        backtrack(combinations, current, i + 1, count + 1, k, index, idxper);
        current[i] = false;
    }
}

void generate_combinations(bool *combinations, int n, int k) {
    bool *current = (bool*)calloc(n, sizeof(bool));
    int *index = (int*)malloc(sizeof(int));
    *index = 0;
    backtrack(combinations, current, 0, 0, k, index, n);
    free(index);
    free(current);
}

//NOTE: itoa is not part of ANSI-C. I compile with GCC on MinGW, which includes it (inherited through UCRT), but not all compilers do
//depending on your compiler, you may have to write a custom implementation
char* padded_base_n(int k, int n, int length) {
    char buffer[33];
    itoa(k,buffer,n);
    int itoa_len = strlen(buffer);
    char *ret = (char*)malloc(length * sizeof(char));
    int pad_count = length - itoa_len;
    for (int i = 0; i < length; i++) {
        if (i < pad_count)
            ret[i] = '0';
        else
            ret[i] = buffer[i - pad_count];
    }
    return ret;
}

void generate_padded_representations(char *padded_representations, int considered, int groups, int selection_length) {
    int string_length = pow(2, considered);
    int write_index = 0;
    for (int i = 0; i < selection_length; i++) {
        char *padded_representation = padded_base_n(i, groups, string_length);
        for (int j = 0; j < string_length; j++)
            padded_representations[write_index++] = padded_representation[j];

        free(padded_representation);
    }
}

int index_from_all_traits(bool *traits, int selection, bool *combinations, int num_vars) {
    int result = 0;
    for (int i = 0; i < num_vars; i++)
        if (combinations[selection * num_vars + i]) {
            result <<= 1;
            if (traits[i])
                result |= 1;
        }
    return result;
}

int get_max_index(double *array, int length) {
    int ret = 0;
    double max = array[0];
    for (int i = 1; i < length; i++) {
        if (array[i] > max) {
            ret = i;
            max = array[i];
        }
    }
    return ret;
}