#include <stdlib.h>
#include "rule_construction.h"
#include "rr.h"
#include "rule_utils.h"

void add_predicate(Rule* r) {
	int n_clauses = count_clauses(r);
	if (n_clauses == 0) return;

	Clause* c = get_clause(r, rand() % n_clauses);
	Predicate* p = malloc(sizeof(Predicate));
	p->feature = rand() % NUM_FEATURES;
	p->value = rand() % 2;
	p->next_predicate = c->first_predicate;

	c->first_predicate = p;
}

void remove_predicate(Rule* r) {
	int n_clauses = count_clauses(r);
	if (n_clauses == 0) return;

	Clause* c = get_clause(r, rand() % n_clauses);
	int n_predicates = count_predicates(c);
	if (n_predicates < 2) return;

	int idx = rand() % n_predicates;

	Predicate* prev = NULL;
	Predicate* cur = c->first_predicate;

	for (int i = 0; i < idx; i++) {
		prev = cur;
		cur = cur->next_predicate;
	}

	if (prev) prev->next_predicate = cur->next_predicate;
	else c->first_predicate = cur->next_predicate;

	free(cur);
}

void flip_value(Rule* r) {
	int n_clauses = count_clauses(r);
	if (n_clauses == 0) return;

	Clause* c = get_clause(r, rand() % n_clauses);
	int n_predicates = count_predicates(c);
	if (n_predicates == 0) return;

	Predicate* p = get_predicate(c, rand() % n_predicates);
	p->value = 1 - p->value;
}

void swap_feature(Rule* r) {
	int n_clauses = count_clauses(r);
	if (n_clauses == 0) return;

	Clause* c = get_clause(r, rand() % n_clauses);
	int n_predicates = count_predicates(c);
	if (n_predicates == 0) return;

	Predicate* p = get_predicate(c, rand() % n_predicates);
	p->feature = rand() % NUM_FEATURES;
}

void add_clause(Rule* r) {
	Clause* c = malloc(sizeof(Clause));
	c->next_clause = r->first_clause;
	r->first_clause = c;

	c->first_predicate = malloc(sizeof(Predicate));
	c->first_predicate->feature = rand() % NUM_FEATURES;
	c->first_predicate->value = rand() % 2;
	c->first_predicate->next_predicate = NULL;
}

void remove_clause(Rule* r) {
	int n_clauses = count_clauses(r);
	if (n_clauses < 2) return;

	int idx = rand() % n_clauses;

	Clause* prev = NULL;
	Clause* cur = r->first_clause;

	for (int i = 0; i < idx; i++) {
		prev = cur;
		cur = cur->next_clause;
	}

	if (prev) prev->next_clause = cur->next_clause;
	else r->first_clause = cur->next_clause;

	Predicate* p = cur->first_predicate;
	while (p) {
		Predicate* tmp = p;
		p = p->next_predicate;
		free(tmp);
	}

	free(cur);
}