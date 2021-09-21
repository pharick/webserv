#include "generateErrorResponse.hpp"

webserv::Response *webserv::generateErrorResponse(unsigned short status,
                                                  const ServerParser::error_pages &error_pages,
                                                  bool send_body,
                                                  const std::string &cookie) {

  Response::headers headers;
  std::iostream *body = NULL;

  headers["Content-Type"] = "text/html";

  ServerParser::error_pages::const_iterator error_page_it = error_pages.find(status);

  if (error_page_it != error_pages.end()) {
    const std::string &path = error_page_it->second;

    struct stat filestat = {};
    stat(path.c_str(), &filestat);

    headers["Content-Length"] = unsignedLongToString(filestat.st_size);

    if (send_body)
      body = new std::fstream(path);
  } else {
    size_t content_length;
    std::stringstream *page = generateErrorPage(&content_length, status);

    headers["Content-Length"] = unsignedLongToString(content_length);

    if (send_body)
      body = page;
    else
      delete page;
  }

  if (!cookie.empty())
    headers["Set-Cookie"] = cookie;

  return (new Response(status, headers, body));
}
