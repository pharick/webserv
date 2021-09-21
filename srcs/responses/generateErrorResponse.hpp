#ifndef WEBSERV_GENERATE_ERROR_RESPONSE_HPP
#define WEBSERV_GENERATE_ERROR_RESPONSE_HPP

#include "config/ServerParser.hpp"
#include "Response.hpp"
#include "pageGenerators.hpp"
#include "responses/utils.hpp"

namespace webserv {

Response *generateErrorResponse(unsigned short status,
                                const ServerParser::error_pages &error_pages = ServerParser::error_pages(),
                                bool send_body = true,
                                const std::string &cookie = std::string());

}

#endif // WEBSERV_GENERATE_ERROR_RESPONSE_HPP
