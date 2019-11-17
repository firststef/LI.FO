#pragma once
#include "Automata.h"

struct RegexTree
{
	char op;
	
	std::shared_ptr<RegexTree> ts;
	std::shared_ptr<RegexTree> td;

	//For automaton conversion
	int assigned_id = -1;
	int i = -1;
	int f = -1;

	std::string to_dot();

	Automaton to_automaton();
};

RegexTree regex_to_regex_tree(std::string regex);
