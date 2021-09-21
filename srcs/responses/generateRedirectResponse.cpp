#include "generateRedirectResponse.hpp"

webserv::Response *webserv::generateRedirectResponse(const LocationParser &location, const std::string &cookie) {
  Response::headers headers;
  headers["Location"] = location.getRedirect();
  headers["Content-Length"] = "0";

  if (!cookie.empty())
    headers["Set-Cookie"] = cookie;

  return (new Response(301, headers, NULL));
}
