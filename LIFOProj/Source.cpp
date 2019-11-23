#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include "BruteForceAlgo.h"
#include "Automata.h"
#include "Regex.h"
#include "GrammarII.h"

using std::ofstream;
using std::string;
using std::endl;
using std::exception;
using std::cout;
using std::cerr;

void save_to_dot(string data, string filename="document")
{
	string name(filename + ".dott");
	ofstream out(name.c_str());
	out << data << endl;
}

void test_minimalistic()
{
	try {

		auto a = automaton1();

		auto res = a.get_rel_table_minimalistic();

		for (auto row : res)
		{
			for (auto col : row) {
				cout << col << " ";
			}
			cout << endl;
		}

		auto new_automaton = a.get_minimal_automaton();

		bool deterministic = a.is_deterministic();

		system("pause");
	}
	catch (exception& ex)
	{
		cerr << "Error occurred: " << ex.what() << endl;
	}
}

void test_deterministic()
{
	try {

		auto a = automaton2();

		auto res = a.get_rel_table_deterministic();

		for (auto row : res)
		{
			cout << "[";
			for (auto el : row.first) {
				cout << el << ",";
			}
			cout << "] : ";

			for (auto lit : row.second) {
				cout << " [";
				for (auto el : lit) {
					cout << el << ",";
				}
				cout << "]";
			}
			cout << endl;
		}

		auto new_automaton = a.get_deterministic_automaton();

		bool deterministic = new_automaton.is_deterministic();

		auto doc = new_automaton.to_dot();

		save_to_dot(doc);

		system("pause");
	}
	catch (exception& ex)
	{
		cerr << "Error occurred: " << ex.what() << endl;
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
	catch (exception& ex)
	{
		cerr << "Error occurred: " << ex.what() << endl;
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
	catch (exception& ex)
	{
		cerr << "Error occurred: " << ex.what() << endl;
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
	catch (exception& ex)
	{
		cerr << "Error occurred: " << ex.what() << endl;
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
	catch (exception& ex)
	{
		cerr << "Error occurred: " << ex.what() << endl;
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
	catch (exception& ex)
	{
		cerr << "Error occurred: " << ex.what() << endl;
	}
};

void test_get_in_chomsky_form()
{
	try {

		GrammarII G{
			{"S", "A"},
		{"a", "b", "c"},
		{"S"},
		{
			{"S", {{"a", "S", "b"},{"c", "A", "c"}}},
			{"A", {{"c"},{ "c", "A"}}}
		}
		};

		auto txt = G.to_text();

		auto Gp = G.get_in_chomsky_form();

		auto txt2 = Gp.to_text(true);

		system("pause");
	}
	catch (exception& ex)
	{
		cerr << "Error occurred: " << ex.what() << endl;
	}
}

void test_cyk()
{
	try {

		GrammarII G{
			{"S", "X", "Y", "Z"},
		{"a", "b", "c"},
		{"S"},
		{
			{"S", {{"X", "Y"}}},
			{"X", {{"a"},{ "X", "Y"}}},
			{"Y", {{"Y", "Z"}, {"a"}, {"b"}}},
			{"Z", {{"c"}}}
		}
		};

		auto txt = G.to_text();

		auto test = G.test_with_cyk("abc");
		auto test2 = G.test_with_cyk("acb");
		auto test3 = G.test_with_cyk("abd");
		auto test4 = G.test_with_cyk("aab");

		system("pause");
	}
	catch (exception& ex)
	{
		cerr << "Error occurred: " << ex.what() << endl;
	}
}

int main(int argc, char* argv[]) {

	try {

		
		
		system("pause");
	}
	catch (exception& ex)
	{
		cerr << "Error occurred: " << ex.what() << endl;
	}
	
	return 1;
}
