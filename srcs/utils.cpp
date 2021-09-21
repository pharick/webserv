#include "utils.hpp"

std::string &webserv::trim(std::string &str) {
  str.erase(0, str.find_first_not_of(" \t\r\n\v\f"));
  str.erase(str.find_last_not_of(" \t\r\n\v\f") + 1);
  return (str);
}

std::string webserv::unsignedLongToString(unsigned long n) {
  std::stringstream str;
  str << n;
  return (str.str());
}

unsigned long webserv::stringToUnsignedlong(const std::string &str) {
  std::stringstream in(str);
  unsigned long n = 0;
  in >> n;
  return (n);
}

unsigned long webserv::stringHEXToUnsignedLong(const std::string &str) {
  std::stringstream in;
  unsigned long n = 0;
  in << std::hex << str;
  in >> n;
  return (n);
}

std::string webserv::unsignedLongToHEXString(unsigned long n) {
  std::stringstream in;
  in << std::hex << n;
  std::string str;
  in >> str;
  return (str);
}

std::string webserv::formatTime(time_t rawtime) {
  struct tm *timeinfo;
  char str[128];

  timeinfo = gmtime(&rawtime);
  strftime(str, 128, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
  return (str);
}
