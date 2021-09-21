#ifndef WEBSERV_GENERATE_REDIRECT_RESPONSE_HPP
#define WEBSERV_GENERATE_REDIRECT_RESPONSE_HPP

#include "Response.hpp"
#include "RequestParser.hpp"
#include "config/ServerParser.hpp"

namespace webserv {

Response *generateRedirectResponse(const LocationParser &location, const std::string &cookie = std::string());

}

#endif //WEBSERV_GENERATE_REDIRECT_RESPONSE_HPP
