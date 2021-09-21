#ifndef WEBSERV_RESPONSES_UTILS_HPP
#define WEBSERV_RESPONSES_UTILS_HPP

#include <string>
#include <sstream>
#include <sys/socket.h>
#include "config/LocationParser.hpp"
#include "RequestParser.hpp"

namespace webserv {

std::string getPath(const webserv::LocationParser &location, const webserv::RequestParser &request);
std::string getUploadPath(const webserv::LocationParser &location, const webserv::RequestParser &request);
std::string findIndex(const std::string &path, const webserv::LocationParser::index_pages &index_pages);

}

#endif // WEBSERV_RESPONSES_UTILS_HPP
