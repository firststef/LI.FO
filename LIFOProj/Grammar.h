#pragma once
#include <vector>
#include <sstream>
#include <set>
#include "Utils.h"
#include <deque>

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
			if (rule.first in n_i) {
				decltype(rule.second) new_right;

				for (auto& concat_symbol : rule.second)
				{
					bool all_valid = true;
					for (auto& single_symbol : concat_symbol)
					{
						if (not(single_symbol in n_i)&& not(single_symbol in terminals))
							all_valid = false;
					}

					if (all_valid)
						new_right.insert(concat_symbol);
				}

				if (not new_right.empty())
					new_rules.push_back(std::make_pair(rule.first, new_right));
			}
		}

		return GrammarII{ n_i,terminals,start_symbol, new_rules };
	}

	GrammarII get_without_inaccessible_symbols()
	{
		std::set<std::string> v_i = {start_symbol};
		std::set<std::string> v_im1;

		do
		{
			v_im1 = v_i;

			for (auto& rule : rules)
			{
				if (not(rule.first in v_im1))
					continue;
				
				for (auto& concat_symbol : rule.second)
				{
					for (auto& single_symbol : concat_symbol)
					{
						v_i.insert(single_symbol);
					}
				}
			}

		} while (v_i != v_im1);

		decltype(non_terminals) new_non_terminals;
		for (auto& s : non_terminals)
		{
			if (s in v_i)
				new_non_terminals.insert(s);
		}

		decltype(terminals) new_terminals;
		for (auto& s : terminals)
		{
			if (s in v_i)
				new_terminals.insert(s);
		}
		
		decltype(rules) new_rules;
		for (auto& rule : rules)
		{
			if (rule.first in v_i) {
				decltype(rule.second) new_right;

				for (auto& concat_symbol : rule.second)
				{
					bool all_valid = true;
					for (auto& single_symbol : concat_symbol)
					{
						if (not(single_symbol in v_i))
							all_valid = false;
					}

					if (all_valid)
						new_right.insert(concat_symbol);
				}

				if (not new_right.empty())
					new_rules.push_back(std::make_pair(rule.first, new_right));
			}
		}

		return GrammarII{ new_non_terminals,new_terminals, start_symbol, new_rules };
	}

	GrammarII get_reduced_form()
	{
		auto g = get_without_unproductive_symbols();
		return g.get_without_inaccessible_symbols();
	}

	GrammarII get_erase_eps_transitions()
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

				//Xi -> Yi -> Xi
				auto new_vec2 = vec;
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
		}

		decltype(non_terminals) new_non_terminals = non_terminals;

		std::string new_start_symbol = start_symbol;
		if (start_symbol in n_i)
		{
			new_start_symbol = start_symbol + std::string("p");
			new_rules.push_back(std::make_pair(new_start_symbol, std::set<std::vector<std::string>>({ { start_symbol},{ G_EPS }})));
			new_non_terminals.insert(new_start_symbol);
		}

		return GrammarII{ new_non_terminals, terminals, new_start_symbol, new_rules };
	}

	GrammarII get_erase_renaming_transitions()
	{
		std::vector<std::set<std::string>> n_classes;
		n_classes.resize(non_terminals.size());

		unsigned class_idx = 0;
		for (auto& nt : non_terminals)
		{
			std::set<std::string> cl_im1;
			n_classes[class_idx] = { nt };

			do
			{
				cl_im1 = n_classes[class_idx];

				for (auto& rule : rules)
				{
					if (not(rule.first in cl_im1))
						continue;
					
					for (auto& concat_symbol : rule.second)
					{
						if (concat_symbol.size() != 1)
							continue;

						for (auto& n : non_terminals)
							if (n in concat_symbol)
								n_classes[class_idx].insert(n);
					}
				}
				
			} while (n_classes[class_idx] != cl_im1);

			class_idx++;
		}

		decltype(rules) new_rules;
		for (auto& rule : rules)
		{
			std::set<std::vector<std::string>> new_left;
			for (auto& concat_symbol : rule.second)
			{
				if (concat_symbol.size() == 1)
				{
					if (not(concat_symbol.front() in non_terminals))
						new_left.insert(concat_symbol);
				}
				else
				{
					new_left.insert(concat_symbol);
				}
			}

			new_rules.push_back(std::make_pair(rule.first, new_left));
		}

		decltype(rules) temp_copy = new_rules;
		for (auto& temp_rule : temp_copy)
		{
			unsigned class_i = 0;
			for (auto & nt : non_terminals)
			{
				if (temp_rule.first != nt && temp_rule.first in n_classes[class_i])
				{
					for (auto& rule : new_rules)
					{
						if (rule.first == nt)
							rule.second.insert(temp_rule.second.begin(), temp_rule.second.end());
					}
				}
				class_i++;
			}
		}

		return GrammarII{ non_terminals, terminals, start_symbol, new_rules };
	}

	bool is_in_chomsky_form()
	{
		for (auto& rule : rules)
		{
			for (auto& concat_symbol : rule.second)
			{
				if (rule.first == start_symbol and concat_symbol.size() == 1 and concat_symbol.front() == std::string(G_EPS))
					continue;
				
				if (concat_symbol.size() == 2)
				{
					for (auto& single_symbol : concat_symbol)
					{
						if (not(single_symbol in non_terminals))
							return false;
					}
				}
				else if (concat_symbol.size() == 2)
				{
					if (not(concat_symbol.front() in terminals))
						return false;
				}
				else
					return false;
			}
		}
		return true;
	}

	GrammarII get_in_chomsky_form()
	{
		auto g = get_reduced_form();
		g = g.get_erase_eps_transitions();
		g = g.get_erase_renaming_transitions();

		std::map<std::string, std::string> new_names;

		auto change_name_everywhere = [&](std::string old_str, std::string new_str)
		{
			for (auto& rule : rules)
			{
				for (auto& concat_symbol : rule.second)
				{
					for (auto& single_symbol : concat_symbol)
					{
						if (single_symbol == old_str)
							const_cast<std::string&>(single_symbol) = new_str;
					}
				}
			}
		};

		unsigned x_i_index = 0;
		for (auto& rule : rules)
		{
			for (auto& concat_symbol : rule.second)
			{
				if (concat_symbol.size() > 1)
					continue;
				
				for (auto& single_symbol : concat_symbol)
				{
					if (single_symbol in terminals)
					{
						auto new_nt = std::string("x") + std::to_string(x_i_index++);
						change_name_everywhere(single_symbol, new_nt);
						non_terminals.insert(new_nt);
						rules.emplace_back(new_nt, std::set<std::vector<std::string>>({ { single_symbol} }));
					}
				}
			}
		}

		unsigned z_i_index = 0;
		for (auto& rule : rules)
		{
			for (auto& concat_symbol : rule.second)
			{
				if (concat_symbol.size() > 2)
					continue;

				std::deque<std::string> row (concat_symbol.begin(), concat_symbol.end());

				concat_symbol = {{row.front(), }}
				
				while( row.size() > 2)
				{
					
				}

				/*for (auto& single_symbol : concat_symbol)
				{
					if (single_symbol in terminals)
					{
						auto new_nt = std::string("x") + std::to_string(x_i_index++);
						change_name_everywhere(single_symbol, new_nt);
						non_terminals.insert(new_nt);
						rules.emplace_back(new_nt, std::set<std::vector<std::string>>({ { single_symbol} }));
					}
				}*/
			}
		}
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
