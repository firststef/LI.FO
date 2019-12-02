
# LI.FO
LiFo is a c++ library made for working with concepts found in the Formal Languages, Automata and Compilers curricula.

LiFo can work with data structures such as grammars, automata etc. and perform different operations on them: minimization, conversion, regex translation etc.

<h3>Example code:</h3>

```c++
INSTANTIATE_STATE_OBJ(s0, NON_FINAL);
INSTANTIATE_STATE_OBJ(s1, NON_FINAL);
INSTANTIATE_STATE_OBJ(s2, NON_FINAL);
INSTANTIATE_STATE_OBJ(s3, NON_FINAL);
INSTANTIATE_STATE_OBJ(s4, FINAL);
INSTANTIATE_STATE_OBJ(s5, FINAL);

Automaton a{ s0, {s0,s1,s2,s3,s4,s5}, {'a', 'b'} };

a.delta(s0, 'a') >> s1;
a.delta(s0, 'b') >> s1;
a.delta(s1, 'a') >> s2;
a.delta(s1, 'b') >> s0;
a.delta(s2, 'a') >> s3;
a.delta(s2, 'b') >> s4;
a.delta(s3, 'a') >> s1;
a.delta(s3, 'b') >> s3;
a.delta(s4, 'a') >> s5;
a.delta(s4, 'b') >> s2;
a.delta(s5, 'a') >> s4;
a.delta(s5, 'b') >> s3;

a.to_dot();
```
This generates the following automaton:
![A1](https://i.imgur.com/kWdTcgO.png "A1")

You can test if a word is accepted by this automaton:

bool res1 = a.test("aabb"); // **false**
bool res2 = a.test("aab"); // **true**

<h3>Some of the functionalities LiFo has:</h3>

**Automata**:

method: is_deterministic();
return object Automaton: get_deterministic_automaton();
return object Automaton: get_minimal_automaton();

method: test(string);
visualization .PNG: to_dot();

**Grammar**:

GrammarIII:
coming soon

GrammarII:

return object GrammarII: get_without_unproductive_symbols();
return object GrammarII: get_without_inaccessible_symbols();
return object GrammarII: get_reduced_form();

return object GrammarII: get_erase_eps_transitions();
return object GrammarII: get_erase_renaming_transitions();

method: is_in_chomsky_form();
return object GrammarII: get_in_chomsky_form();

method: test_with_cyk(string);

visualisation TEXT: to_text();

**Regex**:

return object Automaton: to_automaton();
visualization .PNG: to_dot();

