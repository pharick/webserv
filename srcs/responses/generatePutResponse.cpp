#include "generatePutResponse.hpp"

webserv::Response *webserv::generatePutResponse(const RequestParser &request,
                                                const LocationParser &location,
                                                const std::string &cookie) {
  std::string path = getUploadPath(location, request);

  struct stat filestat = {};
  unsigned short status = stat(path.c_str(), &filestat) < 0 ? 201 : 204;

  if (status == 204)
    std::remove(path.c_str());

  std::rename(request.getBodyPath().c_str(), path.c_str());

  Response::headers headers;
  headers["Content-Length"] = "0";

  if (!cookie.empty())
    headers["Set-Cookie"] = cookie;

  return (new Response(status, headers, NULL));
}
