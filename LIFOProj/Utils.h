#pragma once
#include <vector>
#include <algorithm>

using std::vector;
using std::any_of;
using std::set;

template<typename T>
bool operator | (const T &el, const vector<T>& vec)
{
	return any_of(vec.begin(), vec.end(), [&](const T &e) {return e == el; });
}

template<typename T>
bool operator | (const T &el, const set<T>& vec)
{
	return any_of(vec.begin(), vec.end(), [&](const T &e) {return e == el; });
}

#define in |