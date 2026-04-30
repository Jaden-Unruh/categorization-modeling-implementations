#include "rr.h"

#ifndef RULE_CONSTRUCTION_H
#define RULE_CONSTRUCTION_H

void add_predicate(Rule* r);
void remove_predicate(Rule* r);
void flip_value(Rule* r);
void swap_feature(Rule* r);
void add_clause(Rule* r);
void remove_clause(Rule* r);

#endif