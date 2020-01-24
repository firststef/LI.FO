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

bool Automaton::is_deterministic()
{
	for (auto& map : all_transitions)
	{
		for (auto& literal : alphabet)
		{
			if (map.find(literal) == map.end())
				return false;
		}

		for (auto& pair : map)
		{
			if (pair.first == A_EPS || pair.second.size() != 1)
				return false;
		}
	}
	return true;
}

vector<pair<set<unsigned>, vector<set<unsigned>>>> Automaton::get_rel_table_deterministic()
{
	vector<pair<set<unsigned>, vector<set<unsigned>>>> table;

	table.resize(1);
	unsigned current_row = 0;

	function<set<unsigned>(unsigned,char)> get_next_vector_for_state_literal = [&](unsigned idx, char literal) -> set<unsigned>
	{
		set<unsigned> next_vector;
		auto& next_vec_lit = all_transitions[idx][literal];

		next_vector.insert(next_vec_lit.begin(), next_vec_lit.end());
		for (auto next_state : next_vec_lit)
		{
			auto epsilon_closure = get_next_vector_for_state_literal(next_state, A_EPS);
			next_vector.insert(epsilon_closure.begin(), epsilon_closure.end());
		}

		return next_vector;
	};

	auto initial_closure = get_next_vector_for_state_literal(0, A_EPS);
	initial_closure.insert(0);
	table[0].first = set<unsigned>(initial_closure.begin(), initial_closure.end());

	while (table.back().second.empty())
	{
		decltype(alphabet.begin()) alph_it;
		for (alph_it = alphabet.begin(); alph_it != alphabet.end(); ++alph_it)
		{
			if (*alph_it == A_EPS)
			{
				continue;
			}
			
			set<unsigned> current_set;
			
			for (auto& leaving_state : table[current_row].first)
			{
				auto next_set = get_next_vector_for_state_literal(leaving_state, *alph_it);
				current_set.insert(next_set.begin(), next_set.end());
			}

			table[current_row].second.push_back(current_set);

			bool found = false;
			for (auto& pair : table)
			{
				if (pair.first == current_set)
				{
					found = true;
					break;
				}
			}

			if (not found)
			{
				table.resize(table.size() + 1);
				table.back().first = current_set;
			}
		}

		++current_row;
	}

	return table;
}

Automaton Automaton::get_deterministic_automaton()
{
	auto table = get_rel_table_deterministic();

	vector<shared_ptr<SymbolAutomatonState>> new_states;
	for (auto& pair : table)
	{
		string name("[");
		SymbolAutomatonStateType type = NON_FINAL;
		for (auto& el : pair.first)
		{
			name += to_string(el);
			name += string(",");
			type = SymbolAutomatonStateType(type * all_states[el]->data);
		}
		name += string("]");
		new_states.push_back(make_shared<SymbolAutomatonState>(SymbolAutomatonState{ name, SymbolAutomatonStateType(type) }));
	}

	auto new_alphabet = alphabet;
	alphabet.erase(A_EPS);
	Automaton new_automaton(new_states[0], new_states, alphabet);

	unsigned current_index = 0;
	for(auto& pair : table)
	{
		auto let_it = alphabet.begin();
		for (unsigned letter_num = 0; letter_num < alphabet.size();++letter_num, ++let_it)
		{
			if (*let_it == A_EPS)
				continue;
			
			unsigned next_index_for_new = 0;
			for (auto& pair2 : table)
			{
				if (pair2.first == pair.second[letter_num])
					break;
				next_index_for_new++;
			}
			
			new_automaton.delta(new_states[current_index],  *let_it) >> new_states[next_index_for_new];
		}
		current_index++;
	}

	return new_automaton;
}

vector<vector<bool>> Automaton::get_rel_table_minimalistic()
{
	if (not is_deterministic())
#ifdef _WIN32
        throw exception("Automaton is not deterministic");
#elif __linux__
        throw exception();
#endif

	const auto n = all_states.size();

	//Separable, inseparable table
	vector<vector<bool>> table;

	table.resize(n);
	for (auto& vec : table)
	{
		vec.resize(n);
	}

	//Dependency table
	vector<vector<vector<pair<unsigned, unsigned>>>> dependency;

	dependency.resize(n);
	for (auto& vec : dependency)
	{
		vec.resize(n);
	}

	//Init
	for (unsigned qi = 0; qi < n - 1; qi++)
	{
		for (unsigned qj = qi + 1; qj < n; qj++)
		{
			if (all_states[qi]->data != all_states[qj]->data)
				table[qi][qj] = 1;
			else
				table[qi][qj] = 0;
		}
	}

	function<void(unsigned, unsigned)> update_table = [&](unsigned qi, unsigned qj)
	{
		printf("Updating (%d,%d)\n", qi, qj);
		table[qi][qj] = 1;
		for (auto& dependent_state : dependency[qi][qj])
		{
			if (table[dependent_state.first][dependent_state.second] == 0)
				update_table(dependent_state.first, dependent_state.second);
		}
	};

	for (unsigned qi = 0; qi < n - 1; qi++)
	{
		for (unsigned qj = qi + 1; qj < n; qj++)
		{
			if (table[qi][qj] == 0)
			{
				printf("(%d,%d)\n", qi, qj);

				bool found = false;
				for (auto trans_i : all_transitions[qi])
				{
					for (const auto trans_j : all_transitions[qj])
					{
						if (trans_i.first == trans_j.first)
						{
							for (auto qip : trans_i.second)
							{
								for (auto qjp : trans_j.second)
								{
									//Tuple elements need to be in order
									if (qip > qjp)
									{
										swap(qip, qjp);
									}

									if (table[qip][qjp] == 1)
									{
										printf("Found transition (%d,%d)\n", qip, qjp);
										found = true;
										break;
									}
								}
							}
						}
					}
				}

				if (found)
				{
					update_table(qi, qj);
				}
				else
				{
					for (auto trans_i : all_transitions[qi])
					{
						for (const auto trans_j : all_transitions[qj])
						{
							if (trans_i.first == trans_j.first)
							{
								for (auto qip : trans_i.second)
								{
									for (auto qjp : trans_j.second)
									{
										//Tuple elements need to be in order
										if (qip > qjp)
										{
											swap(qip, qjp);
										}

										printf("With %c to (%d,%d) ", trans_i.first, qip, qjp);

										if (qip != qjp && (qi != qip || qj != qjp) && (qj != qip || qi != qjp))
										{
											printf("Adding to list\n");
											dependency[qip][qjp].emplace_back(make_pair(qi, qj));
										}
										else
											printf("No transition\n");
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return table;
}

Automaton Automaton::get_minimal_automaton()
{
	if (not is_deterministic())
#ifdef _WIN32
		throw exception("Automaton is not deterministic");
#elif __linux__
        throw exception();
#endif

	auto table = get_rel_table_minimalistic();
	const auto n = table[0].size();

	//Classes vector
	vector<vector<unsigned>> classes;
	classes.resize(n);

	//Creating classes
	unsigned diagonal_var = 0;
	for (unsigned i = diagonal_var; i < n; ++i)
	{
		for (unsigned j = diagonal_var; j < n; ++j)
		{
			if (table[i][j] == 0)
				classes[i].push_back(j);
		}
		++diagonal_var;
	}

	auto idx_in_other_classes = [&](unsigned idx)-> bool
	{
		for (unsigned i = 0; i < classes.size(); ++i)
		{
			for (unsigned j = 0; j < classes[i].size(); ++j)
			{
				if (i != idx && classes[i][j] == idx)
					return true;
			}
		}
		return false;
	};

	vector<pair<unsigned, shared_ptr<SymbolAutomatonState>>> indexed_new_states;
	for (unsigned i = 0; i < classes.size(); ++i)
	{
		if (idx_in_other_classes(i))
			continue;

		auto& state = all_states[i];
		auto type = state->data;
		auto name = state->identifier;

		indexed_new_states.emplace_back(make_pair(i, make_shared<SymbolAutomatonState>(SymbolAutomatonState{
			                                               string("[") + name + string("]"), type
		                                               })));
	}

	shared_ptr<SymbolAutomatonState> new_initial_state;
	for (auto class_itr : classes)
	{
		for (auto st_idx : class_itr)
		{
			if (initial == all_states[st_idx])
				new_initial_state = all_states[st_idx];
			break;
		}
	}

	vector<shared_ptr<SymbolAutomatonState>> new_states;
	for (auto& pair : indexed_new_states)
	{
		new_states.push_back(pair.second);
	}

	Automaton new_automaton{new_initial_state, new_states, alphabet};

	auto get_class_for_idx = [&](unsigned idx) -> unsigned
	{
		for (unsigned i = 0; i < classes.size(); ++i)
		{
			for (unsigned j = 0; j < classes[i].size(); ++j)
			{
				if (classes[i][j] == idx)
					return i;
			}
		}
		return 1000;
	};

	for (auto& new_state_pair : indexed_new_states)
	{
		for (auto& literal : alphabet) //looking for all literals
		{
			shared_ptr<SymbolAutomatonState> next_state;
			auto class_idx = get_class_for_idx(new_state_pair.first);
			for (auto& pair : indexed_new_states)
			{
				if (pair.first == class_idx)
				{
					next_state = pair.second;
					break;
				}
			}

			new_automaton.delta(new_state_pair.second, literal) >> next_state;
		}
	}

	return new_automaton;
}

bool Automaton::test(std::string word)
{
	std::function<bool(unsigned, std::shared_ptr<SymbolAutomatonState>)> traverse_function = [&](unsigned idx, std::shared_ptr<SymbolAutomatonState> current_state) -> bool
	{
		if (idx >= word.size() && current_state->data == SymbolAutomatonStateType::FINAL)
			return true;
		
		const auto el = index_map.find(current_state->identifier);
		auto trans = all_transitions[el->second];

		const auto next_set = trans.find(word[idx]);
		if (next_set != trans.end()) {
			for (auto& next : next_set->second)
			{
				if(traverse_function(idx + 1, all_states[next]))
					return true;
			}
		}
		auto next_eps = trans.find(A_EPS);
		if (next_eps != trans.end()) {
			for (auto& next : next_eps->second)
			{
				if (next_eps->first == A_EPS)
					if (traverse_function(idx, all_states[next]))
						return true;
			}
		}

		return false;
	};

	return traverse_function(0, initial);
}

string Automaton::to_dot()
{
	string dot_document;
	dot_document += "digraph A {\n\trankdir = LR;\n";

	ostringstream init_stream;
	ostringstream content_stream;

	for (unsigned idx = 0; idx < all_states.size(); idx++)
	{
		auto& state = all_states[idx];
		init_stream << "\t" << "node" << idx << "[";
		init_stream << "label=\"" << state->identifier << "\"";

		if (state->identifier == initial->identifier)
			init_stream << ", style= bold";
		if (state->data == SymbolAutomatonStateType::FINAL)
			init_stream << ", shape= box";

		init_stream << "]; \n";

		for (auto& trans : all_transitions[idx])
		{
			for (auto& el : trans.second)
			{
				content_stream << "\t" << "node" << idx << " -> " << "node" << el << "[label=\"";
				if (trans.first == A_EPS)
					content_stream << "EPS";
				else
					content_stream << trans.first;
				content_stream << "\"];\n";
			}
		}
	}

	dot_document += init_stream.str();
	dot_document += content_stream.str();
	dot_document += "}";

	return dot_document;
}
