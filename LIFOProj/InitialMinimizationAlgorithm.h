#pragma once
#include <vector>
#include <memory>
#include <functional>

struct Transition
{
	char literal;

	unsigned index;
};

struct State
{
	enum Type
	{
		FINAL,
		NON_FINAL
	} type;

	std::vector<std::pair<std::shared_ptr<State>, Transition>> next;

	State& passes_to(State& next_state, Transition tr)
	{
		next.emplace_back(std::shared_ptr<State>(&next_state), tr);

		return *this;
	}
};

struct Automaton
{
	std::shared_ptr<State> initial;
	std::vector<State> all_states;
};

std::vector<std::vector<bool>> determine_relation(const Automaton& automaton)
{
	auto& all_states = automaton.all_states;

	//Separable, inseparable table
	std::vector<std::vector<bool>> table;
	const auto n = automaton.all_states.size();

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
			if (all_states[qi].type != all_states[qj].type)
				table[qi][qj] = 1;
			else
				table[qi][qj] = 0;
		}
	}

	std::function<void(unsigned, unsigned)> update_table = [&](unsigned qi, unsigned qj)
	{
		table[qi][qj] = 1;
		for (auto dependent_state : dependency[qi][qj])
		{
			if (table[dependent_state.first][dependent_state.second] == 0)
				update_table(dependent_state.first, dependent_state.second);
		}
	};

	for (unsigned qi = 0; qi < n - 1; qi++) {
		for (unsigned qj = qi + 1; qj < n; qj++)
		{
			if (table[qi][qj] == 0) {

				bool found = false;
				for (const auto& lit_i : all_states[qi].next)
				{
					for (const auto& lit_j : all_states[qj].next)
					{
						if (lit_i.second.literal == lit_j.second.literal)
						{
							if (table[lit_i.second.index][lit_j.second.index] == 1)
							{
								found = true;
								break;
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
					for (const auto& lit_i : all_states[qi].next)
					{
						for (const auto& lit_j : all_states[qj].next)
						{
							auto qip = lit_i.second.index;
							auto qjp = lit_j.second.index;
							if ((qip != qjp) && ((qi != qip && qj != qjp) && (qj != qip && qi != qjp)))
							{
								dependency[qip][qjp].emplace_back(std::make_pair(qi, qj));
							}
						}
					}
				}
			}
		}
	}

	return table;
}
