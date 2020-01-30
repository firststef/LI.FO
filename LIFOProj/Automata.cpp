#include "Automata.h"
#include <utility>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <sstream>

using std::exception;
using std::to_string;
using std::function;
using std::swap;
using std::make_pair;
using std::ostringstream;

template <typename StateType, typename AlphabetType, typename TransitionType>
void IAutomaton<StateType, AlphabetType, TransitionType>::Delta::StateCatcher::operator=(shared_ptr<StateType> state)
{
	*this >> state;
}

template <typename StateType, typename AlphabetType, typename TransitionType>
void IAutomaton<StateType, AlphabetType, TransitionType>::Delta::StateCatcher::operator=(unsigned idx)
{
	*this >> idx;
}

template <typename StateType, typename AlphabetType, typename TransitionType>
void IAutomaton<StateType, AlphabetType, TransitionType>::add_state(shared_ptr<StateType> state)
{
	index_map[state->identifier] = all_states.size();
	all_states.push_back(state);
	all_transitions.resize(all_transitions.size() + 1);
}