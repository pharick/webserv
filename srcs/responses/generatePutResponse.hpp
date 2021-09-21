#ifndef WEBSERV_GENERATE_PUT_RESPONSE_HPP
#define WEBSERV_GENERATE_PUT_RESPONSE_HPP

#include <sys/stat.h>
#include "RequestParser.hpp"
#include "config/ServerParser.hpp"
#include "responses/utils.hpp"
#include "Response.hpp"

namespace webserv {

Response *generatePutResponse(const RequestParser &request, const LocationParser &location, const std::string &cookie = std::string());

}

#endif // WEBSERV_GENERATE_PUT_RESPONSE_HPP
