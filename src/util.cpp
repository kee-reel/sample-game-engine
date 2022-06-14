#include <fstream>
#include <sstream>
#include <cstdlib>

#include "util.h"

void error_msg(const char *msg, const char *extra, const char *path)
{
	std::cerr << "ERROR: ";
	if(path)
		 std::cerr << "in file \"" << path << "\", ";
	std::cerr << msg;
	if(extra)
		std::cerr << ", extra info: " << extra;
	std::cerr << std::endl;
}

std::string read_file(const std::string &path)
{
	std::ifstream input;
	input.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	std::stringstream sstr;
	try
	{
		input.open(path);
		sstr << input.rdbuf();
		input.close();
	}
	catch(std::ifstream::failure e)
	{
		std::cerr << "ERROR: can't read file \"" << path << "\":" << e.what() << std::endl;
		return "";
	}
	return sstr.str();
}

std::ostream& operator<<(std::ostream& out, const glm::vec3& v)
{
    return out << '(' << v.x << ' ' << v.y << ' ' << v.z << ')';
}
