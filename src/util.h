#ifndef UTIL_H
#define UTIL_H

void error_msg(const char *msg, const char *extra=nullptr, const char *path=nullptr);
std::string read_file(const std::string &path);

#endif
