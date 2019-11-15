#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include "BruteForceAlgo.h"
#include "Automata.h"

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

int main(int argc, char* argv[]) {

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

		bool deterministic = a.is_deterministic();

		system("pause");
	}
	catch (std::exception& ex)
	{
		std::cerr << "Error occurred: " << ex.what() << std::endl;
	}
	
	return 1;
}
