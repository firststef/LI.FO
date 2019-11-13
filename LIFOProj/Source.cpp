#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include "BruteForceAlgo.h"
#include "Automata.h"


int main(int argc, char* argv[]) {

	INSTANTIATE_STATE_SHARED_PTR(s0, NON_FINAL);
	INSTANTIATE_STATE_SHARED_PTR(s1, NON_FINAL);
	INSTANTIATE_STATE_SHARED_PTR(s2, NON_FINAL);
	INSTANTIATE_STATE_SHARED_PTR(s3, NON_FINAL);
	INSTANTIATE_STATE_SHARED_PTR(s4, FINAL);
	INSTANTIATE_STATE_SHARED_PTR(s5, FINAL);

	/*
	s0->passes_to(s1, Transition{ 'a', 1 });
	s0->passes_to(s1, Transition{ 'b', 1 });
	s1->passes_to(s2, Transition{ 'a', 2 });
	s1->passes_to(s0, Transition{ 'b', 0 });
	s2->passes_to(s3, Transition{ 'a', 3 });
	s2->passes_to(s4, Transition{ 'b', 4 });
	s3->passes_to(s1, Transition{ 'a', 1 });
	s3->passes_to(s3, Transition{ 'b', 3 });
	s4->passes_to(s5, Transition{ 'a', 5 });
	s4->passes_to(s2, Transition{ 'b', 2 });
	s5->passes_to(s4, Transition{ 'a', 4 });
	s5->passes_to(s3, Transition{ 'b', 3 });
	*/
	
	Automaton a{s0, {s0,s1,s2,s3,s4,s5}};

	a[s0]['a'] >> s1;
	a[s0]['b'] >> s1;
	a[s1]['a'] >> s2;
	a[s1]['b'] >> s0;
	a[s2]['a'] >> s3;
	a[s2]['b'] >> s4;
	a[s3]['a'] >> s1;
	a[s3]['b'] >> s3;
	a[s4]['a'] >> s5;
	a[s4]['b'] >> s2;
	a[s5]['a'] >> s4;
	a[s5]['b'] >> s3;

	auto res = determine_relation(a);
	for (auto row : res)
	{
		for (auto col : row) {
			std::cout << col << " ";
		}
		std::cout << std::endl;
	}


	system("pause");
	return 1;
}
