#pragma once
#include <string>
#include <set>
#include <vector>
#include <map>
#include <memory>

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
	map<string, unsigned> index_map;

	struct Delta
	{
		IAutomaton* automaton;

		struct StateCatcher
		{
			Delta* handler;

			unsigned state_index;
			TransitionType literal;

			void operator>>(shared_ptr<StateType> state)
			{
				const auto el = handler->automaton->index_map.find(state->identifier);
				if (el == handler->automaton->index_map.end())
#ifdef _WIN32
					throw std::exception("Target state not in automaton");
#elif __linux__
					throw std::exception();
#endif


				handler->automaton->all_transitions[state_index][literal].insert(el->second);
			}

			void operator>>(unsigned idx)
			{
				if (idx >= handler->automaton->all_states.size())
#ifdef _WIN32
					throw std::exception("Target state not in automaton");
#elif __linux__
					throw std::exception();
#endif

				handler->automaton->all_transitions[state_index][literal].insert(idx);
			}

			void operator=(shared_ptr<StateType> state);
			void operator=(unsigned idx);
		};

		StateCatcher operator()(shared_ptr<StateType> state, TransitionType literal)
		{
			auto element = automaton->index_map.find(state->identifier);
			if (element != automaton->index_map.end())
			{
				bool found = false;
				for (auto alpha : automaton->alphabet)
					if (alpha == literal)
						found = true;

				if (not found)
#ifdef _WIN32
					throw std::exception("Literal not in alphabet");
#elif __linux__
					throw std::exception();
#endif

				StateCatcher catcher{ this, element->second, literal };
				return catcher;
			}
			else
#ifdef _WIN32
				throw std::exception("SymbolAutomatonState does not exist in automaton");
#elif __linux__
				throw std::exception();
#endif
		}
		StateCatcher operator()(unsigned idx, TransitionType literal)
		{
			if (idx < automaton->all_states.size())
			{
				bool found = false;
				for (auto alpha : automaton->alphabet)
					if (alpha == literal)
						found = true;

				if (not found)
					throw std::exception("Literal not in alphabet");

				StateCatcher catcher{ this, idx, literal };
				return catcher;
			}
			else
				throw std::exception("SymbolAutomatonState does not exist in automaton");
		}
	} delta;

	void add_state(shared_ptr<StateType> state);
	void reset_states_identifiers()
	{
		decltype(index_map) new_map;
		unsigned idx = 0;
		for (auto& pair : index_map)
		{
			new_map[std::to_string(idx)] = pair.second;
			all_states[idx]->identifier = std::to_string(idx);
			idx++;
		}
		index_map.clear();
		index_map = new_map;
	}

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