#pragma once
#include <vector>
#include <sstream>
#include <set>
#include <deque>

#include "Utils.h"

#define G_EPS "#"

using std::string;
using std::to_string;
using std::set;
using std::vector;
using std::pair;
using std::function;
using std::map;
using std::deque;
using std::ostringstream;
using std::endl;
using std::move;
using std::back_inserter;

struct GrammarII
{
	set<string> non_terminals;
	set<string> terminals;

	string start_symbol;
	vector < pair < string, set<vector<string>>>> rules;

	GrammarII get_without_unproductive_symbols()
	{
		set<string> n_i;
		set<string> n_im1;

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
					new_rules.emplace_back(make_pair(rule.first, new_right));
			}
		}

		return GrammarII{ n_i,terminals,start_symbol, new_rules };
	}

	GrammarII get_without_inaccessible_symbols()
	{
		set<string> v_i = {start_symbol};
		set<string> v_im1;

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
					new_rules.emplace_back(make_pair(rule.first, new_right));
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
		set<string> n_0;
		for (auto& rule : rules)
		{
			if (vector<string>({  G_EPS  }) in rule.second)
			{
				n_0.insert(rule.first);
			}

		}

		set<string> n_i;
		set<string> n_im1;

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
		function<void(unsigned,unsigned,vector<string>, set<vector<string>>&)> bkt = 
			[&](unsigned idx, unsigned max_size, vector<string> vec, set<vector<string>>& set) // idx = 0, maxsize = vec.size()
		{
			if (idx == max_size)
			{
				if (not vec.empty() && vec != vector<string>({G_EPS}))
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

		string new_start_symbol = start_symbol;
		if (start_symbol in n_i)
		{
			new_start_symbol = start_symbol + string("p");
			new_rules.emplace_back(make_pair(new_start_symbol, set<vector<string>>({ { start_symbol},{ G_EPS }})));
			new_non_terminals.insert(new_start_symbol);
		}

		return GrammarII{ new_non_terminals, terminals, new_start_symbol, new_rules };
	}

	GrammarII get_erase_renaming_transitions()
	{
		vector<set<string>> n_classes;
		n_classes.resize(non_terminals.size());

		unsigned class_idx = 0;
		for (auto& nt : non_terminals)
		{
			set<string> cl_im1;
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
			set<vector<string>> new_left;
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

			new_rules.emplace_back(make_pair(rule.first, new_left));
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
				if (rule.first == start_symbol and concat_symbol.size() == 1 and concat_symbol.front() == string(G_EPS))
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

		auto change_name_everywhere = [](string old_str, string new_str, GrammarII& g)
		{
			for (auto& rule : g.rules)
			{
				decltype(rule.second) new_set;
				
				for (auto& concat_symbol : rule.second)
				{
					vector<string> new_concat_symbol;
					
					for (auto& single_symbol : concat_symbol)
					{
						if (single_symbol == old_str)
							new_concat_symbol.push_back(new_str);
						else
							new_concat_symbol.push_back(single_symbol);
					}

					new_set.insert(new_concat_symbol);
				}

				rule.second = new_set;
			}
		};

		unsigned x_i_index = 0;

		bool loop_when_modified = true;
		while (loop_when_modified)
		{
			loop_when_modified = false;

			auto search = [&]() {
				for (auto& rule : g.rules)
				{
					for (auto& concat_symbol : rule.second)
					{
						if (concat_symbol.size() == 1 && concat_symbol.front() in g.terminals)
							continue;

						for (const auto single_symbol : concat_symbol)
						{
							if (single_symbol in g.terminals)
							{
								auto new_nt = "x" + to_string(x_i_index++);
								change_name_everywhere(single_symbol, new_nt, g);
								g.non_terminals.insert(new_nt);
								g.rules.emplace_back(new_nt, set<vector<string>>({ { single_symbol} }));
								loop_when_modified = true;

								return;
							}
						}
					}
				}
			};
			search();
		}

		decltype(g.rules) new_rules;
		unsigned z_i_index = 0;
		for (auto& rule : g.rules)
		{
			decltype(rule.second) new_set;

			for (auto& concat_symbol : rule.second)
			{
				vector<string> new_concat_symbol;

				if (concat_symbol.size() <= 2)
				{
					new_set.insert(concat_symbol);
					continue;
				}

				deque<string> row(concat_symbol.begin(), concat_symbol.end());

				while (row.size() > 2)
				{
					auto back1 = row.back();
					row.pop_back();
					auto back2 = row.back();
					row.pop_back();

					auto new_z_nonterm = "z" + to_string(z_i_index++);

					g.non_terminals.insert(new_z_nonterm);
					new_rules.emplace_back(new_z_nonterm, set<vector<string>>({ { back2, back1 } }));
					row.emplace_back(new_z_nonterm);
				}
				
				new_set.insert({ row.begin(), row.end() });
			}

			rule.second = new_set;
		}
		move(new_rules.begin(), new_rules.end(), back_inserter(g.rules));

		return g;
	}
	
	string to_text(bool prettify = false)
	{
		if (prettify)
		{
			decltype(rules) new_rule_order;
			std::sort(rules.begin(), rules.end(), [&](
			                                   decltype(rules)::value_type el1, decltype(rules)::value_type el2) ->bool
			{
				if (el1.first == start_symbol)
					return true;
				if (el2.first == start_symbol)
					return false;

				const auto counter = [&](decltype(rules)::value_type el) 
				{
					unsigned count = 0;
					for (auto& concat_symbol : el.second)
						count += concat_symbol.size();
					return count;
				};
				
				return  counter(el1) != counter(el2) ? counter(el1) >= counter(el2) : el1.first < el2.first;
			});
		}
		
		ostringstream text;

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
					text << " ";
				}
				if (rule.second.size() > 1 && idx <= rule.second.size() - 2)
					text << "|";
				idx++;
			}

			text << endl;
		}

		return text.str();
	}
};
