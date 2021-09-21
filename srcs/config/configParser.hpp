#ifndef WEBSERV_CONFIG_PARSER_HPP
#define WEBSERV_CONFIG_PARSER_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <list>
#include "config/ServerParser.hpp"
#include "configParserExceptions.hpp"

namespace webserv {

std::list<ServerParser *> parseConfig(const std::string &path);
void printConfig(const std::list<webserv::ServerParser *> &servers);

}

#endif // WEBSERV_CONFIG_PARSER_HPP
