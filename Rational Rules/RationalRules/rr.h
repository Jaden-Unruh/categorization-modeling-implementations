#ifndef RR_H
#define RR_H

#define NUM_FEATURES 4

typedef struct Predicate {
	int feature; //index/i
	int value; //0 or 1, rather than a boolean
	struct Predicate* next_predicate;
} Predicate;

typedef struct Clause {
	Predicate* first_predicate;
	struct Clause* next_clause;
} Clause;

typedef struct Rule {
	Clause* first_clause;
} Rule;

typedef struct Example {
	int features[NUM_FEATURES];
	int label;
	struct Example* next_example;
} Example;

double log_beta(int* counts, int k);
double log_prior(Rule* rule);
int evaluate_rule(Rule* rule, int* features);
double log_likelihood(Rule* rule, Example* first_example, double b);
double log_posterior(Rule* rule, Example* first_example, double b);
Rule* propose(Rule* r);
Rule* sample_initial_rule();
Rule* run_mcmc(Example* data, int steps, double b);

#endif