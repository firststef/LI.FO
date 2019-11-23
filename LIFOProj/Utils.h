#pragma once
#include <vector>
#include <algorithm>

template<typename T>
bool operator | (const T &el, const std::vector<T>& vec)
{
	return std::any_of(vec.begin(), vec.end(), [&](const T &e) {return e == el; });
}

template<typename T>
bool operator | (const T &el, const std::set<T>& vec)
{
	return std::any_of(vec.begin(), vec.end(), [&](const T &e) {return e == el; });
}

#define in |