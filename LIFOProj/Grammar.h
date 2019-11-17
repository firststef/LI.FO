#pragma once
#include <vector>
#include <sstream>
#include <set>
#include "Utils.h"

#define G_EPS "#"

struct GrammarII
{
	std::set<std::string> non_terminals;
	std::set<std::string> terminals;

	std::string start_symbol;
	std::vector < std::pair < std::string, std::set<std::vector<std::string>>>> rules;

	GrammarII get_without_unproductive_symbols()
	{
		std::set<std::string> n_i;
		std::set<std::string> n_im1;

		do
		{
			n_im1 = n_i;

			for (auto& rule : rules)
			{
				for (auto& concat_symbol : rule.second)
				{
					bool all_valid = true;
					for (auto& single_symbol : concat_symbol)
					{
						bool found = single_symbol in n_i || single_symbol in terminals || single_symbol == G_EPS;

						if (not found)
						{
							all_valid = false;
							break;
						}
					}

					if (all_valid)
					{
						n_i.insert(rule.first);
						break;
					}
				}
			}
			
		} while (n_i != n_im1);

		decltype(rules) new_rules;
		for (auto& rule : rules)
		{
			if (rule.first in n_i)
				new_rules.push_back(rule);
		}

		return GrammarII{ n_i,terminals,start_symbol, new_rules };
	}

	GrammarII get_erase_e_transition()
	{
		std::set<std::string> n_0;
		for (auto& rule : rules)
		{
			if (std::vector<std::string>({  G_EPS  }) in rule.second)
			{
				n_0.insert(rule.first);
			}

		}


		std::set<std::string> n_i;
		std::set<std::string> n_im1;

		do
		{
			n_im1 = n_i;

			for (auto& rule : rules)
			{
				if (not(rule.first in non_terminals))
					continue;
				
				for (auto& concat_symbol : rule.second)
				{
					bool all_valid = true;
					for (auto& single_symbol : concat_symbol)
					{
						if (not(single_symbol in n_im1) && not(single_symbol == G_EPS))
							all_valid = false;
					}

					if (all_valid)
					{
						n_i.insert(rule.first);
						break;
					}
				}
			}

		} while (n_i != n_im1);

		unsigned new_idx = 0;
		std::map<std::string, std::string> new_names;
		std::function<void(unsigned,unsigned,std::vector<std::string>, std::set<std::vector<std::string>>&)> bkt = 
			[&](unsigned idx, unsigned max_size, std::vector<std::string> vec, std::set<std::vector<std::string>>& set) // idx = 0, maxsize = vec.size()
		{
			if (idx == max_size)
			{
				if (not vec.empty() && vec != std::vector<std::string>({G_EPS}))
					set.insert(vec);
				return;
			}

			if (vec[idx] in n_i)
			{
				//Xi -> eps
				auto new_vec1 = vec;
				new_vec1.erase(new_vec1.begin() + idx);
				bkt(idx, max_size - 1, new_vec1, set);

				//Xi -> Yi
				auto new_vec2 = vec;
				if (new_names.find(vec[idx]) == new_names.end())
				{
					new_names[vec[idx]] = std::string("Y") + std::to_string(new_idx++);
				}
				new_vec2[idx] = new_names[vec[idx]];
				bkt(idx + 1, max_size, new_vec2, set);
			}
			else
				bkt(idx + 1, max_size, vec, set);
			
		};

		decltype(rules) new_rules = rules;
		for (auto& rule : new_rules)
		{
			decltype(rule.second) new_set;

			for (auto& concat_symbol : rule.second)
			{
				bkt(0, concat_symbol.size(), concat_symbol, new_set);
			}

			rule.second.clear();
			rule.second = new_set;

			if (new_names.find(rule.first) != new_names.end())
			{
				rule.first = new_names[rule.first];
			}
		}

		decltype(non_terminals) new_non_terminals;
		for (auto& nt : non_terminals)
		{
			if (new_names.find(nt) != new_names.end())
			{
				new_non_terminals.insert(new_names[nt]);
			}
			else
				new_non_terminals.insert(nt);
		}

		std::string new_start_symbol = start_symbol;
		if (start_symbol in n_i)
		{
			new_start_symbol = start_symbol + std::string("p");
			new_rules.push_back(std::make_pair(new_start_symbol, std::set<std::vector<std::string>>({ { start_symbol},{ G_EPS }})));
			new_non_terminals.insert(new_start_symbol);
		}

		return GrammarII{ new_non_terminals, terminals, new_start_symbol, new_rules };
	}
	
	std::string to_text()
	{
		std::ostringstream text;

		for (auto& rule : rules)
		{
			text << rule.first << " -> ";

			unsigned idx = 0;
			for (auto& concat_symbol : rule.second)
			{
				for (auto& single_symbol : concat_symbol)
				{
					if (single_symbol == "#")
						text << "eps";
					else
						text << single_symbol;
				}
				if (rule.second.size() > 1 && idx <= rule.second.size() - 2)
					text << "|";
				idx++;
			}

			text << std::endl;
		}

		return text.str();
	}
};
