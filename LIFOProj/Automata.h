#pragma once
#include <utility>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <map>
#include <set>
#include <unordered_set>

#define EPS 0

struct State
{
	std::string identifier;
	
	enum Type
	{
		FINAL = 0,
		NON_FINAL = 1
	} type;
};

#define INSTANTIATE_STATE_OBJ(state, type) auto state = std::make_shared<State>(State{#state, State::type});

struct Automaton
{
	std::shared_ptr<State> initial;
	std::vector<std::shared_ptr<State>> all_states;

	std::set<char> alphabet;
	
	std::vector<std::map<char, std::set<unsigned>>> all_transitions;

	//Map for indexing states
	std::map<std::string, unsigned> map;

	struct Delta
	{
		Automaton* automaton;

		struct StateCatcher
		{
			Delta* handler;

			unsigned state_index;
			char literal;

			void operator>>(std::shared_ptr<State> state);

			void operator=(std::shared_ptr<State> state);
		};

		StateCatcher operator()(std::shared_ptr<State> state, char literal);
	} delta;

	Automaton(std::shared_ptr<State> initial, std::vector<std::shared_ptr<State>> list, std::set<char> alphabet);

	void add_state(std::shared_ptr<State> state);

	bool is_deterministic();

	std::vector<std::pair<std::set<unsigned>, std::vector<std::set<unsigned>>>> get_rel_table_deterministic();
	Automaton get_deterministic_automaton();

	std::vector<std::vector<bool>> get_rel_table_minimalistic();
	Automaton get_minimal_automaton();
};

inline Automaton automaton1()
{
	INSTANTIATE_STATE_OBJ(s0, NON_FINAL);
	INSTANTIATE_STATE_OBJ(s1, NON_FINAL);
	INSTANTIATE_STATE_OBJ(s2, NON_FINAL);
	INSTANTIATE_STATE_OBJ(s3, NON_FINAL);
	INSTANTIATE_STATE_OBJ(s4, FINAL);
	INSTANTIATE_STATE_OBJ(s5, FINAL);

	Automaton a{ s0, {s0,s1,s2,s3,s4,s5}, {'a', 'b'} };

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

inline Automaton automaton2()
{
	INSTANTIATE_STATE_OBJ(s0, NON_FINAL);
	INSTANTIATE_STATE_OBJ(s1, NON_FINAL);
	INSTANTIATE_STATE_OBJ(s2, NON_FINAL);
	INSTANTIATE_STATE_OBJ(s3, NON_FINAL);
	INSTANTIATE_STATE_OBJ(s4, FINAL);

	Automaton a{ s0, {s0,s1,s2,s3,s4}, {'a', 'b', EPS} };

	a.delta(s0, 'a') >> s1;
	a.delta(s0, 'b') >> s2;
	a.delta(s1, 'a') >> s3;
	a.delta(s1, 'a') >> s2;
	a.delta(s1, EPS) >> s0;
	a.delta(s2, 'a') >> s3;
	a.delta(s2, 'b') >> s0;
	a.delta(s3, 'a') >> s4;
	a.delta(s3, 'b') >> s3;
	a.delta(s4, EPS) >> s1;

	return a;
}