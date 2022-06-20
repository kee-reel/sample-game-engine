#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

void error_msg(const char *msg, const char *extra=nullptr, const char *path=nullptr);
std::string read_file(const std::string &path);
std::ostream& operator<<(std::ostream& out, const glm::vec3& v);
template<class T>
T get_value(nlohmann::basic_json<>& config, const char *name)
{
    auto iter = config.find(name);
    if(iter == config.end())
        return T{};
    return *iter;
}

#endif
