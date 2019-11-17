#pragma once
#include <vector>

template<typename T>
bool operator | (const T &str, const std::vector<T>& vec)
{
	for (auto& each : vec)
	{
		if (str == each)
			return true;
	}
	return false;
}

template<typename T>
bool operator | (const T &str, const std::set<T>& vec)
{
	for (auto& each : vec)
	{
		if (str == each)
			return true;
	}
	return false;
}

#define in |