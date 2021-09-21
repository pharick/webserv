#ifndef WEBSERV_UTILS_HPP
#define WEBSERV_UTILS_HPP

#include <string>
#include <sstream>

namespace webserv {

std::string &trim(std::string &str);
std::string unsignedLongToString(unsigned long n);
unsigned long stringToUnsignedlong(const std::string &str);
unsigned long stringHEXToUnsignedLong(const std::string &str);
std::string unsignedLongToHEXString(unsigned long n);
std::string formatTime(time_t rawtime);

}

#endif // WEBSERV_UTILS_HPP
