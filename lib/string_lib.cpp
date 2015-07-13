#include<string>
#include"string_lib.hpp"
std::string trim(std::string const& str)
{
	if (str.empty())
		return str;
	std::size_t first = str.find_first_not_of(" \r\t\n");
	std::size_t last = str.find_last_not_of(" \r\t\n");
	return str.substr(first, last - first + 1);
}
