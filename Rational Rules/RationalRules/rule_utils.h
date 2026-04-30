#include "rr.h"

#ifndef RULE_UTILS_H
#define RULE_UTILS_H

Predicate* copy_predicate(Predicate* p);
Clause* copy_clause(Clause* c);
Rule* deep_copy(Rule* r);
void free_rule(Rule* r);
void print_rule(Rule* r);
int count_clauses(Rule* r);
Clause* get_clause(Rule* r, int idx);
int count_predicates(Clause* c);
Predicate* get_predicate(Clause* c, int idx);

#endif