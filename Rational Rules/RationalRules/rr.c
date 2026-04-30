#include <math.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "rr.h"
#include "rule_construction.h"
#include "rule_utils.h"

//log probability functions

double log_beta(int* counts, int k) {
	double sum_lgamma = 0.0;
	int sum = 0;

	for (int i = 0; i < k; i++) {
		sum_lgamma += lgamma(counts[i]);
		sum += counts[i];
	}

	return sum_lgamma - lgamma(sum);
}

double log_prior(Rule* rule) {
	int D_counts[2] = {0, 0};
	int C_counts[2] = {0, 0};

	int* P_counts = calloc(NUM_FEATURES, sizeof(int));

	int** F_counts = malloc(NUM_FEATURES * sizeof(int*));
	for (int i = 0; i < NUM_FEATURES; i++)
		F_counts[i] = calloc(2, sizeof(int));

	int num_clauses = 0;
	
	for (Clause* c = rule->first_clause; c != NULL; c = c->next_clause) {
		num_clauses++;

		int num_predicates = 0;

		for (Predicate* p = c->first_predicate; p != NULL; p = p->next_predicate) {
			num_predicates++;

			P_counts[p->feature]++;
			F_counts[p->feature][p->value]++;
		}

		if (num_predicates > 0) {
			C_counts[0] += num_predicates - 1;
			C_counts[1]++;
		}
	}

	if (num_clauses > 0) {
		D_counts[0] = num_clauses - 1;
		D_counts[1] = 1;
	}

	// compute log prior
	double logp = 0.0;

	// D
	{ 
		int alpha[2] = {D_counts[0] + 1, D_counts[1] + 1};
		int base[2] = {1, 1};
		logp += log_beta(alpha, 2) - log_beta(base, 2);
	}

	// C
	{
		int alpha[2] = {C_counts[0] + 1, C_counts[1] + 1};
		int base[2] = {1, 1};
		logp += log_beta(alpha, 2) - log_beta(base, 2);
	}

	// P
	{
		int* alpha = malloc(NUM_FEATURES * sizeof(int));
		int* base = malloc(NUM_FEATURES * sizeof(int));

		for (int i = 0; i < NUM_FEATURES; i++) {
			alpha[i] = P_counts[i] + 1;
			base[i] = 1;
		}

		logp += log_beta(alpha, NUM_FEATURES) - log_beta(base, NUM_FEATURES);

		free(alpha);
		free(base);
	}

	// F_i
	for (int i = 0; i < NUM_FEATURES; i++) {
		int alpha[2] = {
			F_counts[i][0] + 1,
			F_counts[i][1] + 1
		};
		int base[2] = {1, 1};

		logp += log_beta(alpha, 2) - log_beta(base, 2);
	}

	free(P_counts);
	for (int i = 0; i < NUM_FEATURES; i++)
		free(F_counts[i]);
	free(F_counts);

	return logp;
}

int evaluate_rule(Rule* rule, int* features) {
	for (Clause* c = rule->first_clause; c != NULL; c = c->next_clause) {
		int clause_true = 1;

		for (Predicate* p = c->first_predicate; p != NULL; p = p->next_predicate) {
			if (features[p->feature] != p->value) {
				clause_true = 0;
				break;
			}
		}

		if (clause_true) return 1;
	}

	return 0;
}

double log_likelihood(Rule* rule, Example* first_example, double b) {
	int errors = 0;

	for (Example* example = first_example; example != NULL; example = example->next_example) {
		int prediction = evaluate_rule(rule, example->features);
		if (prediction != example->label)
			errors++;
	}

	return -b * errors;
}

double log_posterior(Rule* rule, Example* first_example, double b) {
	return log_prior(rule) + log_likelihood(rule, first_example, b);
}

Rule* propose(Rule* r) {
	Rule* new_r = deep_copy(r);

	int move = rand() % 6;
	switch (move) {
		case 0: add_predicate(new_r); break;
		case 1: remove_predicate(new_r); break;
		case 2: flip_value(new_r); break;
		case 3: swap_feature(new_r); break;
		case 4: add_clause(new_r); break;
		case 5: remove_clause(new_r); break;
	}

	return new_r;
}

Rule* sample_initial_rule() {
	Rule* r = malloc(sizeof(Rule));

	Clause* c = malloc(sizeof(Clause));
	r->first_clause = c;
	c->next_clause = NULL;

	Predicate* p = malloc(sizeof(Predicate));
	p->feature = rand() % NUM_FEATURES;
	p->value = rand() % 2;
	p->next_predicate = NULL;

	c->first_predicate = p;

	return r;
}

Rule* run_mcmc(Example* data, int steps, double b) {
	Rule* current = sample_initial_rule();
	double logp_current = log_posterior(current, data, b);

	Rule* best = deep_copy(current);
	double best_logp = logp_current;

	for (int t = 0; t < steps; t++) {
		Rule* proposal = propose(current);
		double logp_proposal = log_posterior(proposal, data, b);

		double diff = logp_proposal - logp_current;

		double u = log((double)rand() / RAND_MAX);

		if (u < diff) {
			free_rule(current);
			current = proposal;
			logp_current = logp_proposal;

			if (logp_current > best_logp) {
				free_rule(best);
				best = deep_copy(current);
				best_logp = logp_current;
			}
		} else {
			free_rule(proposal);
		}

		if(t % 1000 == 0) {
			printf("step %d logp=%.3f best=%.3f\n", t, logp_current, best_logp);
		}
	}

	free_rule(current);
	return best;
}