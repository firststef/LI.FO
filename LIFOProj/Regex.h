#pragma once
#include "SymbolAutomaton.h"
#include "Automata.h"

struct RegexTree
{
	char op;
	
	shared_ptr<RegexTree> ts;
	shared_ptr<RegexTree> td;

	//For automaton conversion
	int assigned_id = -1;
	int i = -1;
	int f = -1;

	string to_dot();

	SymbolAutomaton to_automaton();
};

RegexTree regex_to_regex_tree(string regex);
