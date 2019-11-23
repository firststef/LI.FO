#pragma once
#include <vector>
#include <set>

#define G_EPS "#"

using std::string;
using std::set;
using std::vector;
using std::pair;

struct GrammarII
{
	set<string> non_terminals;
	set<string> terminals;

	string start_symbol;
	vector < pair < string, set<vector<string>>>> rules;

	GrammarII get_without_unproductive_symbols();

	GrammarII get_without_inaccessible_symbols();

	GrammarII get_reduced_form();

	GrammarII get_erase_eps_transitions();

	GrammarII get_erase_renaming_transitions();

	bool is_in_chomsky_form();

	GrammarII get_in_chomsky_form();

	bool test_with_cyk(std::string word);

	string to_text(bool prettify = false);
};
