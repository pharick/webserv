#include "generateStaticResponse.hpp"

webserv::Response *webserv::generateStaticResponse(const RequestParser &request,
                                                   const ServerParser &server,
                                                   const LocationParser &location,
                                                   const std::string &uri,
                                                   const std::string &cookie) {
  std::string path = getPath(location, request);

  struct stat filestat = {};
  if (stat(path.c_str(), &filestat) != 0)
    return (generateErrorResponse(404, server.getErrorPages(), request.getMethod() != HEAD));

  if (filestat.st_mode & S_IFDIR) {
    std::string index_path = findIndex(path, location.getIndexPages());
    if (!index_path.empty())
      return (generateFileResponse(index_path, request.getMethod() != HEAD, cookie));

    if (location.isAutoindex())
      return (generateAutoindexResponse(path, uri, request.getMethod() != HEAD, cookie));

    return (generateErrorResponse(404, server.getErrorPages(), request.getMethod() != HEAD));
  }

  return (generateFileResponse(path, request.getMethod() != HEAD, cookie));
}

webserv::Response *webserv::generateFileResponse(const std::string &path, bool send_body, const std::string &cookie) {
  Response::headers headers;
  std::iostream *body = NULL;
  std::string extension = path.substr(path.find_last_of('.') + 1);
  std::string mime_type;

  try {
    mime_type = MimeTypes::getInstance().find(extension);
  } catch (MimeTypes::ExtensionDoesNotExistsException &e) {
    mime_type = "text/plain";
  }

  struct stat filestat = {};
  stat(path.c_str(), &filestat);

  headers["Content-Type"] = mime_type;
  headers["Content-Length"] = unsignedLongToString(filestat.st_size);
  headers["Last-Modified"] = formatTime(filestat.st_mtimespec.tv_sec);

  if (send_body)
    body = new std::fstream(path);

  if (!cookie.empty())
    headers["Set-Cookie"] = cookie;

  return (new Response(200, headers, body));
}

webserv::Response *webserv::generateAutoindexResponse(const std::string &path,
                                                      const std::string &uri,
                                                      bool send_body,
                                                      const std::string &cookie) {
  Response::headers headers;
  std::iostream *body = NULL;

  size_t content_length;
  std::iostream *page = generateAutoindexPage(&content_length, path, uri);

  headers["Content-Type"] = "text/html";
  headers["Content-Length"] = unsignedLongToString(content_length);

  if (send_body)
    body = page;

  if (!cookie.empty())
    headers["Set-Cookie"] = cookie;

  return (new Response(200, headers, body));
}
