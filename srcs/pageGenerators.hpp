#ifndef WEBSERV_PAGE_GENERATORS_HPP
#define WEBSERV_PAGE_GENERATORS_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <dirent.h>
#include "references/Statuses.hpp"
#include "utils.hpp"

namespace webserv {

std::stringstream *generateErrorPage(size_t *len, unsigned short status);
std::stringstream *generateAutoindexPage(size_t *len, const std::string &path, const std::string &uri);

}

#endif // WEBSERV_PAGE_GENERATORS_HPP
