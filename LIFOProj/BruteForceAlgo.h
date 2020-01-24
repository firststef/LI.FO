#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstring>

constexpr auto MAX_RULES = 25;
constexpr auto MAX_WORD_LENGTH = 3;

using NonTerminal = char;
using Terminal = char;

struct Symbol {
	union
	{
		NonTerminal nt;
		Terminal t;
	} value;
};

struct Unit
{
	enum UnitType {
		Single,
		Double
	} type;

	Symbol field[2];
};

struct Rule
{
	NonTerminal left;

	std::vector<Unit> rightList;
};

bool is_alphanumeric(char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'));
}

bool is_capital(char c)
{
	return c >= 'A' && c <= 'Z';
}

bool parse_rules(FILE* file, Rule rule_list[], unsigned & num_rules)
{
	char string[100];

	unsigned rule_index = 0;
	while (fgets(string, 100, file))
	{
		if (string[0] == '\n' || string[0] == '\0' || (string[0] == '/' && string[1] == '/'))
			continue;

		Rule rule;

		unsigned index = 0;

		if (!is_capital(string[0]))
			return false;

		rule.left = string[0];

		if (string[1] != '-' || string[2] != '>')
			return false;

		for (unsigned i = 3; i < strlen(string);)
		{
			if (!is_alphanumeric(string[i]))
				return false;

			if (string[i + 1] == '|' || string[i + 1] == '\n' || string[i + 1] == '\0') {

				rule.rightList.push_back(Unit{ Unit::Single, {{string[i]}, {'\0'}} });

				i += 2;
			}
			else if (string[i + 2] == '|' || string[i + 2] == '\n' || string[i + 2] == '\0') {
				if (!is_alphanumeric(string[i + 1]))
					return false;

				rule.rightList.push_back(Unit{ Unit::Double, {{string[i]}, {string[i + 1]}} });

				i += 3;
			}
			else
				return false;
		}

		if (rule.rightList.empty())
			return false;

		rule_list[rule_index] = rule;
		rule_index++;
	}
	num_rules = rule_index + 1;

	return true;
}

void run_automata_func(Rule rule, Rule rule_list[], unsigned num_rules, unsigned level, char* word)
{
	if (memcmp(word, "18", 3) == 0)
		auto stop = 0;

	if (level >= MAX_WORD_LENGTH)
	{
		word[level] = '\0';
		printf("%s\n", word);
		return;
	}

	for (auto& unit : rule.rightList)
	{
		if (unit.type == Unit::Single)
		{
			if (is_capital(unit.field->value.nt))
			{
				for (unsigned ir = 0; ir < num_rules; ++ir)
				{
					auto& newRule = rule_list[ir];

					if (newRule.left == unit.field->value.nt)
						run_automata_func(newRule, rule_list, num_rules, level + 1, word);
				}
			}
			else
			{
				word[level] = unit.field->value.t;
				printf("%s\n", word);
			}
		}
		else
		{
			if (level + 1 >= MAX_WORD_LENGTH)
				return;

			word[level] = unit.field->value.t;

			for (unsigned ir = 0; ir < num_rules; ++ir)
			{
				auto& newRule = rule_list[ir];

				if (newRule.left == unit.field[1].value.nt)
					run_automata_func(newRule, rule_list, num_rules, level + 1, word);
			}
		}
	}

	word[level] = '\0';
}

void execute_list(Rule rule_list[], unsigned num_rules)
{
	for (unsigned ir = 0; ir < num_rules; ++ir)
	{
		auto& rule = rule_list[ir];

		if (rule.left == 'S')
		{
			char* word = new char[MAX_WORD_LENGTH + 1];
			memset(word, '\0', MAX_WORD_LENGTH + 1);

			run_automata_func(rule, rule_list, num_rules, 0, word);

			delete[] word;
		}
	}
}

void generate_words(int argc, char* argv[])
{
	FILE* fin;
	if (argc > 1)
		fin = fopen(argv[1], "r");
	else
		fin = fopen("reguli.txt", "r");

	Rule rules[MAX_RULES];
	unsigned num_rules = 0;
	parse_rules(fin, rules, num_rules);

	execute_list(rules, num_rules);

	fclose(fin);
}