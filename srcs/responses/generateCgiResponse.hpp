#ifndef WEBSERV_GENERATE_CGI_RESPONSE_HPP
#define WEBSERV_GENERATE_CGI_RESPONSE_HPP

#include "CgiResponse.hpp"
#include "RequestParser.hpp"
#include "config/LocationParser.hpp"
#include "Connections.hpp"
#include "responses/generateErrorResponse.hpp"
#include "responses/utils.hpp"

namespace webserv {

IResponse *generateCgiResponse(const RequestParser &request,
                               const ServerParser &server,
                               const LocationParser &location,
                               const t_client &client,
                               const std::string &cookie = std::string());

}

#endif // WEBSERV_GENERATE_CGI_RESPONSE_HPP
