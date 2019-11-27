#pragma once
#include <vector>
#include <algorithm>
#include <map>

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

//template<typename T, template<typename C> class V, typename C>
//bool operator | (const T &el, const std::map<T, V<C>>& map)
//{
//	return map.find(el) != map.end();
//}

#define in |