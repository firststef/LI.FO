#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include "BruteForceAlgo.h"
#include "Automata.h"
#include "Regex.h"
#include "Grammar.h"

void save_to_dot(std::string data, std::string filename="document")
{
	std::string name(filename + ".dott");
	std::ofstream out(name.c_str());
	out << data << std::endl;
}

void test_minimalistic()
{
	try {

		auto a = automaton1();

		auto res = a.get_rel_table_minimalistic();

		for (auto row : res)
		{
			for (auto col : row) {
				std::cout << col << " ";
			}
			std::cout << std::endl;
		}

		auto new_automaton = a.get_minimal_automaton();

		bool deterministic = a.is_deterministic();

		system("pause");
	}
	catch (std::exception& ex)
	{
		std::cerr << "Error occurred: " << ex.what() << std::endl;
	}
}

void test_deterministic()
{
	try {

		auto a = automaton2();

		auto res = a.get_rel_table_deterministic();

		for (auto row : res)
		{
			std::cout << "[";
			for (auto el : row.first) {
				std::cout << el << ",";
			}
			std::cout << "] : ";

			for (auto lit : row.second) {
				std::cout << " [";
				for (auto el : lit) {
					std::cout << el << ",";
				}
				std::cout << "]";
			}
			std::cout << std::endl;
		}

		auto new_automaton = a.get_deterministic_automaton();

		bool deterministic = new_automaton.is_deterministic();

		auto doc = new_automaton.to_dot();

		save_to_dot(doc);

		system("pause");
	}
	catch (std::exception& ex)
	{
		std::cerr << "Error occurred: " << ex.what() << std::endl;
	}
}

void test_regex_to_automaton()
{
	try {

		auto t = regex_to_regex_tree("a|(b*.c)");

		auto a = t.to_automaton();
		
		a = a.get_deterministic_automaton();
		a.reset_states_identifiers();
		
		a.get_minimal_automaton();
		a.reset_states_identifiers();

		auto doc = a.to_dot();
		save_to_dot(doc);
	}
	catch (std::exception& ex)
	{
		std::cerr << "Error occurred: " << ex.what() << std::endl;
	}
}

void test_remove_unproductive_symbols()
{
	try {
		GrammarII G{
			{"S", "A", "B", "C"},
		{"a", "b", "c"},
		{"S"},
		{
			{"S", {{"a"}, {"a", "A"}, {"b", "C"}}},
			{"A", {{"a", "A", "B"}}},
			{"B", {{"b", "a", "c"}}},
			{"C", {{"a", "S", "b"}}}
		}
		};

		auto str = G.to_text();

		auto Gp = G.get_without_unproductive_symbols();

		auto str2 = Gp.to_text();
		
		system("pause");
	}
	catch (std::exception& ex)
	{
		std::cerr << "Error occurred: " << ex.what() << std::endl;
	}
}

void test_get_reduced_form()
{
	try {
		GrammarII G{
			{"S", "A", "B", "C"},
		{"a", "b", "c"},
		{"S"},
		{
			{"S", {{"a"}, {"a", "A"},{ "b", "C"}}},
			{"A", {{"a", "A", "B"}}},
			{"B", {{"b", "a", "c"}}},
			{"C", {{"a", "S", "b"}}}
		}
		};

		auto str = G.to_text();

		auto Gp = G.get_without_unproductive_symbols();

		auto str1 = Gp.to_text();

		auto Gpp = Gp.get_without_inaccessible_symbols();

		auto str2 = Gpp.to_text();

		system("pause");
	}
	catch (std::exception& ex)
	{
		std::cerr << "Error occurred: " << ex.what() << std::endl;
	}
}

void test_remove_eps_rules()
{
	try {
		GrammarII G{
			{"S", "A", "B", "C"},
		{"a", "b", "c"},
		{"S"},
		{
			{"S", {{"a", "A", "b", "C"}, {"B", "C"}}},
			{"A", {{"a", "A"},{"a", "B"}}},
			{"B", {{"b", "B"},{"C"}}},
			{"C", {{"c", "C"},{ G_EPS}}}
		}
		};

		auto str = G.to_text();

		auto Gp = G.get_erase_eps_transitions();

		auto str2 = Gp.to_text();

		system("pause");
	}
	catch (std::exception& ex)
	{
		std::cerr << "Error occurred: " << ex.what() << std::endl;
	}
}

void test_erase_renaming_transitions()
{
	try {
		GrammarII G{
			{"x", "y", "z"},
		{"a", "b", "c"},
		{"x"},
		{
			{"x", {{"y"}, {"a", "x"},{"a"}}},
			{"y", {{"z"},{ "b", "y"}, {"b"}}},
			{"z", {{"c", "z"},{ "c"}}}
		}
		};

		auto str = G.to_text();

		auto Gp = G.get_erase_renaming_transitions();

		auto str2 = Gp.to_text();

		system("pause");
	}
	catch (std::exception& ex)
	{
		std::cerr << "Error occurred: " << ex.what() << std::endl;
	}
};


int main(int argc, char* argv[]) {

	try {
		
		
		system("pause");
	}
	catch (std::exception& ex)
	{
		std::cerr << "Error occurred: " << ex.what() << std::endl;
	}
	
	return 1;
}
