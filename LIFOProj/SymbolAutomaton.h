#pragma once
#include "Automata.h"

enum SymbolAutomatonStateType
{
	FINAL = 0,
	NON_FINAL = 1
};

using SymbolAutomatonState = State<SymbolAutomatonStateType>;

#define INSTANTIATE_STATE_OBJ(state, type) auto state = make_shared<SymbolAutomatonState>(SymbolAutomatonState{#state, type});

struct SymbolAutomaton : IAutomaton<SymbolAutomatonState, char, char>
{
	SymbolAutomaton(shared_ptr<SymbolAutomatonState> initial, vector<shared_ptr<SymbolAutomatonState>> list, set<char> alphabet)
		:IAutomaton<State<SymbolAutomatonStateType>, char, char>(initial, list, alphabet)
	{}

	bool is_deterministic();

	vector<pair<set<unsigned>, vector<set<unsigned>>>> get_rel_table_deterministic();
	SymbolAutomaton get_deterministic_automaton();

	vector<vector<bool>> get_rel_table_minimalistic();
	SymbolAutomaton get_minimal_automaton();

	bool test(std::string word);

	string to_dot();
};

inline SymbolAutomaton automaton1()
{
	INSTANTIATE_STATE_OBJ(s0, NON_FINAL);
	INSTANTIATE_STATE_OBJ(s1, NON_FINAL);
	INSTANTIATE_STATE_OBJ(s2, NON_FINAL);
	INSTANTIATE_STATE_OBJ(s3, NON_FINAL);
	INSTANTIATE_STATE_OBJ(s4, FINAL);
	INSTANTIATE_STATE_OBJ(s5, FINAL);

	SymbolAutomaton a{ s0, {s0,s1,s2,s3,s4,s5}, {'a', 'b'} };

	a.delta(s0, 'a') >> s1;
	a.delta(s0, 'b') >> s1;
	a.delta(s1, 'a') >> s2;
	a.delta(s1, 'b') >> s0;
	a.delta(s2, 'a') >> s3;
	a.delta(s2, 'b') >> s4;
	a.delta(s3, 'a') >> s1;
	a.delta(s3, 'b') >> s3;
	a.delta(s4, 'a') >> s5;
	a.delta(s4, 'b') >> s2;
	a.delta(s5, 'a') >> s4;
	a.delta(s5, 'b') >> s3;

	return a;
}

inline SymbolAutomaton automaton2()
{
	INSTANTIATE_STATE_OBJ(s0, NON_FINAL);
	INSTANTIATE_STATE_OBJ(s1, NON_FINAL);
	INSTANTIATE_STATE_OBJ(s2, NON_FINAL);
	INSTANTIATE_STATE_OBJ(s3, NON_FINAL);
	INSTANTIATE_STATE_OBJ(s4, FINAL);

	SymbolAutomaton a{ s0, {s0,s1,s2,s3,s4}, {'a', 'b', A_EPS} };

	a.delta(s0, 'a') >> s1;
	a.delta(s0, 'b') >> s2;
	a.delta(s1, 'a') >> s3;
	a.delta(s1, 'a') >> s2;
	a.delta(s1, A_EPS) >> s0;
	a.delta(s2, 'a') >> s3;
	a.delta(s2, 'b') >> s0;
	a.delta(s3, 'a') >> s4;
	a.delta(s3, 'b') >> s3;
	a.delta(s4, A_EPS) >> s1;

	return a;
}