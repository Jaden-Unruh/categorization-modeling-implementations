#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "rr.h"
#include "rule_utils.h"

void main() {
	//seed rand to ensure difference
	srand(time(NULL));

	Example* e1 = (Example *)malloc(sizeof(Example));
	Example* e2 = (Example *)malloc(sizeof(Example));
	Example* e3 = (Example *)malloc(sizeof(Example));
	Example* e4 = (Example *)malloc(sizeof(Example));
	Example* e5 = (Example *)malloc(sizeof(Example));
	Example* e6 = (Example *)malloc(sizeof(Example));
	Example* e7 = (Example *)malloc(sizeof(Example));
	Example* e8 = (Example *)malloc(sizeof(Example));
	Example* e9 = (Example *)malloc(sizeof(Example));
	Example* e10 = (Example *)malloc(sizeof(Example));

	int features1[4] = {1, 1, 1, 1};
	int features2[4] = {0, 1, 1, 1};
	int features3[4] = {0, 0, 1, 0};
	int features4[4] = {0, 0, 0, 0};
	int features5[4] = {0, 1, 0, 1};
	int features6[4] = {1, 1, 1, 0};
	int features7[4] = {1, 1, 0, 1};
	int features8[4] = {1, 0, 0, 1};
	int features9[4] = {1, 1, 0, 0};
	int features10[4] = {1, 1, 0, 0};

	memcpy(e1->features, features1, sizeof(e1->features));
	memcpy(e2->features, features2, sizeof(e2->features));
	memcpy(e3->features, features3, sizeof(e3->features));
	memcpy(e4->features, features4, sizeof(e4->features));
	memcpy(e5->features, features5, sizeof(e5->features));
	memcpy(e6->features, features6, sizeof(e6->features));
	memcpy(e7->features, features7, sizeof(e7->features));
	memcpy(e8->features, features8, sizeof(e8->features));
	memcpy(e9->features, features9, sizeof(e9->features));
	memcpy(e10->features, features10, sizeof(e10->features));
	
	e1->label = 1;
	e2->label = 1;
	e3->label = 0;
	e4->label = 0;
	e5->label = 1;
	e6->label = 1;
	e7->label = 1;
	e8->label = 0;
	e9->label = 0;
	e10->label = 0;

	e1->next_example = e2;
	e2->next_example = e3;
	e3->next_example = e4;
	e4->next_example = e5;
	e5->next_example = e6;
	e6->next_example = e7;
	e7->next_example = e8;
	e8->next_example = e9;
	e9->next_example = e10;
	e10->next_example = NULL;

	Rule* best = run_mcmc(e1, 10000, 5.0);
	print_rule(best);
}
