#ifndef WEBSERV_GENERATE_STATIC_RESPONSE_HPP
#define WEBSERV_GENERATE_STATIC_RESPONSE_HPP

#include <string>
#include "responses/utils.hpp"
#include "responses/generateErrorResponse.hpp"
#include "config/ServerParser.hpp"
#include "references/MimeTypes.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "pageGenerators.hpp"

namespace webserv {

Response *generateStaticResponse(const RequestParser &request,
                                 const ServerParser &server,
                                 const LocationParser &location,
                                 const std::string &uri,
                                 const std::string &cookie = std::string());
Response *generateFileResponse(const std::string &path, bool send_body, const std::string &cookie = std::string());
Response *generateAutoindexResponse(const std::string &path,
                                    const std::string &uri,
                                    bool send_body,
                                    const std::string &cookie = std::string());

}

#endif // WEBSERV_GENERATE_STATIC_RESPONSE_HPP
