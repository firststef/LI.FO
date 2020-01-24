#pragma once
#include <string>
#include <set>
#include <vector>
#include <map>

#define A_EPS 0

using std::string;
using std::shared_ptr;
using std::set;
using std::vector;
using std::map;
using std::set;
using std::pair;
using std::make_shared;

template<typename Data>
struct State
{
	string identifier;
	
	Data data;
};

template<typename StateType, typename AlphabetType, typename TransitionType>
struct IAutomaton
{
	shared_ptr<StateType> initial;
	vector<shared_ptr<StateType>> all_states;

	set<AlphabetType> alphabet;

	vector<map<TransitionType, set<unsigned>>> all_transitions;

	//Map for indexing states
	map<string, unsigned> map;

	struct Delta
	{
		IAutomaton* automaton;

		struct StateCatcher
		{
			Delta* handler;

			unsigned state_index;
			char literal;

			void operator>>(shared_ptr<StateType> state);

			void operator>>(unsigned idx);

			void operator=(shared_ptr<StateType> state);
			void operator=(unsigned idx);
		};

		StateCatcher operator()(shared_ptr<StateType> state, TransitionType literal);
		StateCatcher operator()(unsigned idx, TransitionType literal);
	} delta;

	void add_state(shared_ptr<StateType> state);
	void reset_states_identifiers();

	IAutomaton(shared_ptr<StateType> initial, vector<shared_ptr<StateType>> list, set<AlphabetType> alphabet)
		:initial(initial), alphabet(alphabet), delta({ this })
	{
		this->add_state(initial);
		for (auto& st : list)
		{
			if (st->identifier != initial->identifier)
				this->add_state(st);
		}
	}
};

enum SymbolAutomatonStateType
{
	FINAL = 0,
	NON_FINAL = 1
};

using SymbolAutomatonState = State<SymbolAutomatonStateType>;

#define INSTANTIATE_STATE_OBJ(state, type) auto state = make_shared<SymbolAutomatonState>(SymbolAutomatonState{#state, type});

struct Automaton : IAutomaton<SymbolAutomatonState, char, char>
{
	Automaton(shared_ptr<SymbolAutomatonState> initial, vector<shared_ptr<SymbolAutomatonState>> list, set<char> alphabet)
	:IAutomaton<State<SymbolAutomatonStateType>, char, char>(initial, list, alphabet)
	{}
	
	bool is_deterministic();

	vector<pair<set<unsigned>, vector<set<unsigned>>>> get_rel_table_deterministic();
	Automaton get_deterministic_automaton();

	vector<vector<bool>> get_rel_table_minimalistic();
	Automaton get_minimal_automaton();

	bool test(std::string word);

	string to_dot();
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

	Automaton a{ s0, {s0,s1,s2,s3,s4}, {'a', 'b', A_EPS} };

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