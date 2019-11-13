#pragma once
#include <utility>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <map>

struct State
{
	std::string identifier;
	
	enum Type
	{
		FINAL,
		NON_FINAL
	} type;
};

#define INSTANTIATE_STATE_SHARED_PTR(state, type) auto state = std::make_shared<State>(State{#state, State::type});

struct Automaton
{
	std::shared_ptr<State> initial;
	std::vector<std::shared_ptr<State>> all_states;

	std::vector<std::map<char, std::vector<unsigned>>> all_transitions;

	//Map for indexing states
	std::map<std::string, unsigned> map;

	struct TransitionHandler
	{
		unsigned state_index;
		Automaton* automaton;

		struct StateCatcher
		{
			TransitionHandler* handler;
			char literal;
			
			void operator>>(std::shared_ptr<State> state)
			{
				auto new_state_idx = handler->automaton->map.find(state->identifier)->second;

				handler->automaton->all_transitions[handler->state_index][literal].push_back(new_state_idx);
			}

			void operator=(std::shared_ptr<State> state)
			{
				*this >> state;
			}
			
		};
		
		StateCatcher operator[](char literal)
		{
			StateCatcher catcher{this, literal};
			return catcher;
		}
	};

	Automaton(std::shared_ptr<State> initial, std::initializer_list<std::shared_ptr<State>> list)
	{
		this->initial = initial;
		for (auto& st : list)
		{
			add_state(st);
		}
	}

	void add_state(std::shared_ptr<State> state)
	{
		auto element = map.find(state->identifier);
		if (element == map.end())
		{
			map[state->identifier] = all_states.size();
			all_states.push_back(state);
			all_transitions.resize(all_transitions.size() + 1);
		}
		else
			throw std::exception("State already exists in automaton");
	}

	TransitionHandler operator[](std::shared_ptr<State> state)
	{
		auto element = map.find(state->identifier);
		if (element != map.end())
		{
			return TransitionHandler{ element->second, this };
		}
		else
			throw std::exception("State does not exist in automaton");
	}
};

std::vector<std::vector<bool>> determine_relation(const Automaton& automaton)
{
	auto& all_states = automaton.all_states;
	auto& all_transitions = automaton.all_transitions;
	const auto n = automaton.all_states.size();
	
	//Separable, inseparable table
	std::vector<std::vector<bool>> table;

	table.resize(n);
	for (auto& vec : table)
	{
		vec.resize(n);
	}

	//Dependency table
	std::vector<std::vector<std::vector<std::pair<unsigned, unsigned>>>> dependency;

	dependency.resize(n);
	for (auto& vec : dependency)
	{
		vec.resize(n);
	}

	//Init
	for (unsigned qi = 0; qi < n - 1; qi++) {
		for (unsigned qj = qi + 1; qj < n; qj++) {
			if (all_states[qi]->type != all_states[qj]->type)
				table[qi][qj] = 1;
			else
				table[qi][qj] = 0;
		}
	}

	std::function<void(unsigned,unsigned)> update_table = [&](unsigned qi, unsigned qj)
	{
		printf("Updating (%d,%d)\n", qi, qj);
		table[qi][qj] = 1;
		for ( auto& dependent_state : dependency[qi][qj])
		{
			if (table[dependent_state.first][dependent_state.second] == 0)
				update_table(dependent_state.first, dependent_state.second);
		}
	};

	for (unsigned qi = 0; qi < n - 1; qi++) {
		for (unsigned qj = qi + 1; qj < n; qj++)
		{
			if (table[qi][qj] == 0) {

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
										std::swap(qip, qjp);
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
											std::swap(qip, qjp);
										}

										printf("With %c to (%d,%d) ", trans_i.first, qip, qjp);

										if (qip != qjp && (qi != qip || qj != qjp) && (qj != qip || qi != qjp))
										{
											printf("Adding to list\n");
											dependency[qip][qjp].emplace_back(std::make_pair(qi, qj));
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

//Automaton get_minimal()
