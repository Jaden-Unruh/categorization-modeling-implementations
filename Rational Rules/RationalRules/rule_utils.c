#include <stdlib.h>
#include <stdio.h>
#include "rule_utils.h"
#include "rr.h"

//variable names (first NUM_FEATURES are only used for printing, otherwise referred to by indices 0,...,NUM_FEATURES-1)
char var_names[] = "PQRSTUVW";

Predicate* copy_predicate(Predicate* p) {
	if (!p) return NULL;

	Predicate* head = NULL;
	Predicate* tail = NULL;

	for (; p != NULL; p = p-> next_predicate) {
		Predicate* new_p = malloc(sizeof(Predicate));
		new_p->feature = p->feature;
		new_p->value = p->value;
		new_p->next_predicate = NULL;

		if (!head)
			head = tail = new_p;
		else {
			tail->next_predicate = new_p;
			tail = new_p;
		}
	}

	return head;
}

Clause* copy_clause(Clause* c) {
	if (!c) return NULL;

	Clause* head = NULL;
	Clause* tail = NULL;

	for (; c!= NULL; c= c->next_clause) {
		Clause* new_c = malloc(sizeof(Clause));
		new_c->first_predicate = copy_predicate(c->first_predicate);
		new_c->next_clause = NULL;

		if (!head)
			head = tail = new_c;
		else {
			tail->next_clause = new_c;
			tail = new_c;
		}
	}

	return head;
}

Rule* deep_copy(Rule* r) {
	Rule* new_r = malloc(sizeof(Rule));
	new_r->first_clause = copy_clause(r->first_clause);
	return new_r;
}

void free_rule(Rule* r) {
	if (!r) return;

	Clause* c = r->first_clause;
	while (c) {
		Clause* next_c = c->next_clause;

		Predicate* p = c->first_predicate;
		while (p) {
			Predicate* next_p = p->next_predicate;
			free(p);
			p = next_p;
		}
		free(c);
		c = next_c;
	}
	free(r);
}

void print_rule(Rule* r) {
	if (!r || !r->first_clause) {
		printf("False\n");
		return;
	}

	Clause* c = r->first_clause;
	int first_clause = 1;

	while(c) {
		if (!first_clause)
			printf(" v ");
		first_clause = 0;

		printf("(");

		Predicate* p = c->first_predicate;
		int first_predicate = 1;

		if (!p)
			printf("True");

		while(p) {
			if (!first_predicate)
				printf(" & ");
			first_predicate = 0;

			if (!p->value)
				printf("!");

			printf("%c", var_names[p->feature]);

			p = p->next_predicate;
		}

		printf(")");

		c = c->next_clause;
	}

	printf("\n");
}

int count_clauses(Rule* r) {
	int count = 0;
	for (Clause* c = r->first_clause; c; c = c->next_clause) count++;
	return count;
}

Clause* get_clause(Rule* r, int idx) {
	Clause* c = r->first_clause;
	for (int i = 0; i < idx && c; i++) c = c->next_clause;
	return c;
}

int count_predicates(Clause* c) {
	int count = 0;
	for (Predicate* p = c->first_predicate; p; p = p->next_predicate) count++;
	return count;
}

Predicate* get_predicate(Clause* c, int idx) {
	Predicate* p = c->first_predicate;
	for (int i = 0; i < idx && p; i++) p = p->next_predicate;
	return p;
}