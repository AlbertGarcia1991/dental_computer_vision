#include <iostream>
#include <fstream>

bool isFile(std::string src)
{
	std::ifstream file;
	file.open(src);
	if (file) {
		return true;
	}
	throw std::runtime_error("The given file does not exist");
	return false;
}
