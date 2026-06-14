#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "functions.h"

#define NUM_VARS 5
#define CONSIDERED_VARS 3
#define OUTPUT_GROUPS 4
#define CONFIDENCE_FACTOR 0.1

void learn_correct_categorization(double *weights, int total_length, bool *combinations, char *padded_representations, int selections, int selection_length, bool *traits, char group) {
    int representation_length = pow(2, CONSIDERED_VARS);
    for (int selection = 0; selection < selections; selection++) {
        int selIdx = index_from_all_traits(traits, selection, combinations, NUM_VARS);
        for (int i = 0; i < selection_length; i++)
            if (!(padded_representations[i * representation_length + selIdx] == group))
                weights[selection * selection_length + i] *= CONFIDENCE_FACTOR;
    }
    normalize(weights, total_length);
}

double* get_prediction(double *weights, int total_length, bool *combinations, char *padded_representations, int selections, int selection_length, bool *traits) {
    double* ret = (double*)calloc(OUTPUT_GROUPS, sizeof(double));
    int representation_length = pow(2, CONSIDERED_VARS);
    for (int selection = 0; selection < selections; selection++) {
        int selIdx = index_from_all_traits(traits, selection, combinations, NUM_VARS);
        for (int i = 0; i < selection_length; i++)
            ret[padded_representations[i * representation_length + selIdx] - '0'] += weights[selection * selection_length + i];
    }
    return ret;
}

int main() {

    int selections = choose(NUM_VARS, CONSIDERED_VARS),
    selection_length = (int) pow(OUTPUT_GROUPS, pow(2, CONSIDERED_VARS)),
    total_length = selections * selection_length;

    bool *combinations = (bool*)calloc(selections * NUM_VARS, sizeof(bool));

    generate_combinations(combinations, NUM_VARS, CONSIDERED_VARS);

    char *padded_representations = (char*)calloc(selection_length * pow(2, CONSIDERED_VARS), sizeof(char));

    generate_padded_representations(padded_representations, CONSIDERED_VARS, OUTPUT_GROUPS, selection_length);

    double *weights = (double*)calloc(total_length, sizeof(double));

    FILE *file = fopen("aliendatasimple.csv", "r"); //prompt for input
    FILE *output = fopen("aliendataout.csv", "w"); //prompt for input
    int trials_per_test = 104; //prompt for input
    char line[16];
    int test_count = 0;

    for (int i = 0; i < OUTPUT_GROUPS; i++)
        fprintf(output, "Group %d weight, ", i);

    fprintf(output, "Prediction\n");

    while (true) {
        test_count++;
        printf("test %d\n", test_count);

        reset(weights, total_length);

        int lines_read = 0;

        for (int i = 0; i < trials_per_test && fgets(line, sizeof(line), file); i++) {
            lines_read++;
            bool traits[NUM_VARS]; //TODO: use indices based on num vars. For now assuming 5.
            for (int j = 0; j < NUM_VARS; j++)
                traits[j] = line[j + 4] == '1';

            double *prediction = get_prediction(weights, total_length, combinations, padded_representations, selections, selection_length, traits);
            int max_index = get_max_index(prediction, OUTPUT_GROUPS);

            for (int i = 0; i < OUTPUT_GROUPS; i++)
                fprintf(output, "%f, ", prediction[i]);
            fprintf(output, "%d\n", max_index);
            
            free(prediction);

            char correct_categorization = line[10] - 1; //accounting for 1-indexed group inputs. Fix input file later.

            learn_correct_categorization(weights, total_length, combinations, padded_representations, selections, selection_length, traits, correct_categorization);
        }

        if (lines_read == 0)
            break;
    }

    fclose(file);
    fclose(output);

    return(0);
}