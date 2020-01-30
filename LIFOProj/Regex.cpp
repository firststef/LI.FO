#include <deque>
#include <sstream>
#include <functional>

#include "Regex.h"
#include "SymbolAutomaton.h"

using std::function;

template struct IAutomaton<State<SymbolAutomatonStateType >, char, char>;

std::string RegexTree::to_dot()
{
	std::string dot_document;
	dot_document += "digraph A {\n";

	std::ostringstream init_stream;
	std::ostringstream content_stream;

	std::function<void(RegexTree&, std::string)> dfs = [&](RegexTree& tree, std::string state)
	{
		init_stream << "\t" << "node" << state << "[";
		init_stream << "label=\"" << tree.op << "\"]; \n";

		if (tree.ts)
		{
			content_stream << "\t" << "node" << state << " -> " << "node" << state + "s" << ";\n";
			dfs(*tree.ts, state + "s");
		}

		if (tree.td)
		{
			content_stream << "\t" << "node" << state << " -> " << "node" << state + "d" << ";\n";
			dfs(*tree.td, state + "d");
		}
	};

	dfs(*this, "0");

	dot_document += init_stream.str();
	dot_document += content_stream.str();
	dot_document += "}";

	return dot_document;
}

RegexTree regex_to_regex_tree(std::string regex)
{
	unsigned i = 0;
	std::deque<char> STIVA1;
	std::deque<std::shared_ptr<RegexTree>> STIVA2;

	auto prec = [](char c) -> unsigned
	{
		switch (c)
		{
		case '|':
			return 1;
		case '.':
			return 2;
		case '*':
			return 3;
		case '(':
			return 0;
		default:
			return 10;
		}
	};

	auto build_tree = [&]()
	{
		char op = STIVA1.front();
		STIVA1.pop_front();
		std::shared_ptr<RegexTree> t1 = STIVA2.front();
		STIVA2.pop_front();
		switch (op)
		{
		case '*':
			{
				auto t = std::make_shared<RegexTree>(RegexTree{op, t1, NULL});
				STIVA2.push_front(t);
				break;
			}
		case '|':
		case '.':
			{
				auto t2 = STIVA2.front();
				STIVA2.pop_front();
				auto t = std::make_shared<RegexTree>(RegexTree{op, t2, t1});
				STIVA2.push_front(t);
				break;
			}
		}
	};

	while (i < regex.size())
	{
		char c = regex[i];
		switch (c)
		{
		case '(':
			{
				STIVA1.push_front(c);
				break;
			}

		case '*':
		case '|':
		case '.':
			{
				while (not STIVA1.empty() && prec(STIVA1.front()) >= prec(c))
					build_tree();
				STIVA1.push_front(c);
				break;
			}
		case ')':
			{
				do
				{
					build_tree();
				}
				while (STIVA1.front() != '(');
				STIVA1.pop_front();
				break;
			}
		default:
			{
				STIVA2.push_front(std::make_shared<RegexTree>(RegexTree{c, NULL, NULL}));
				break;
			}
		}
		i++;
	}
	while (not STIVA1.empty())
		build_tree();
	std::shared_ptr<RegexTree> t = STIVA2.front();
	STIVA2.pop_front();

	return *t;
}

SymbolAutomaton RegexTree::to_automaton()
{
	unsigned idx = 1;
	std::set<char> alphabet;
	std::function<void(RegexTree& tree)> pre = [&](RegexTree& tree)
	{
		if (tree.op != '.')
		{
			tree.assigned_id = idx++;
			if (tree.op != '*' && tree.op != '|')
				alphabet.insert(tree.op);
		}

		if (tree.ts)
			pre(*tree.ts);
		if (tree.td)
			pre(*tree.td);
	};
	pre(*this);

	std::vector<std::shared_ptr<SymbolAutomatonState>> new_states;
	for (unsigned st = 1; st <= 2 * (idx-1); ++st)
	{
		new_states.push_back(std::make_shared<SymbolAutomatonState>(SymbolAutomatonState{ std::to_string(st), SymbolAutomatonStateType::NON_FINAL }));
	}

	alphabet.insert(A_EPS);
	SymbolAutomaton new_automaton(new_states[0], new_states, alphabet);//SymbolAutomaton initial q init is awkward

	auto& x = new_automaton.delta;
	auto z = x(1, 'a');

	std::function<void(RegexTree& tree)> post = [&](RegexTree& tree)
	{
		if (tree.ts)
			post(*tree.ts);

		if (tree.td)
			post(*tree.td);

		if (tree.op != '.')
		{
			tree.f = tree.assigned_id * 2;
			tree.i = tree.f - 1;
		}
		else
		{
			tree.i = tree.ts->i;
			tree.f = tree.td->f;
		}
	};
	post(*this);

	std::function<void(RegexTree& tree)> pre2 = [&](RegexTree& tree)
	{
        new_automaton.delta(tree.i - 1, A_EPS) >> (tree.ts->i - 1);
		switch (tree.op)
		{
		case '|':
			new_automaton.delta(tree.i - 1, A_EPS) >> (tree.ts->i - 1);
			new_automaton.delta(tree.i - 1, A_EPS) >> (tree.td->i - 1);
			new_automaton.delta(tree.ts->f - 1, A_EPS) >> (tree.f - 1);
			new_automaton.delta(tree.td->f - 1, A_EPS) >> (tree.f - 1);
			break;
		case '.':
			new_automaton.delta(tree.ts->f - 1, A_EPS) >> (tree.td->i - 1);
			break;
		case '*':
			new_automaton.delta(tree.i - 1, A_EPS) >> (tree.f - 1);
			new_automaton.delta(tree.i - 1, A_EPS) >> (tree.ts->i - 1);
			new_automaton.delta(tree.ts->f - 1, A_EPS) >> (tree.ts->i - 1);
			new_automaton.delta(tree.ts->f - 1, A_EPS) >> (tree.f - 1);
			break;
		default:
			new_automaton.delta(tree.i - 1, tree.op) >> (tree.f - 1);
		}

		if (tree.ts)
			pre2(*tree.ts);
		if (tree.td)
			pre2(*tree.td);
	};
	pre2(*this);

	new_automaton.initial = new_states[this->i - 1];
	new_automaton.all_states[this->f - 1]->data = FINAL;

	return new_automaton;
}
