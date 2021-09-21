#include "generateCgiResponse.hpp"

static void setCgiEnvironment(const webserv::RequestParser &request,
                              const webserv::t_client &client,
                              const webserv::LocationParser &location) {
  setenv("SERVER_SOFTWARE", "Sysoev", 1);

  std::string host = request.getHeader("Host");
  if (!host.empty())
    setenv("SERVER_NAME", host.c_str(), 1);
  else
    setenv("SERVER_NAME", inet_ntoa(*reinterpret_cast<const in_addr *>(&client.server_host)), 1);

  setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
  setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
  setenv("SERVER_PORT", webserv::unsignedLongToString(client.server_port).c_str(), 1);
  setenv("REQUEST_METHOD", webserv::Methods::getInstance().findName(request.getMethod()).c_str(), 1);
  setenv("REQUEST_URI", request.getUri().c_str(), 1);

  setenv("SCRIPT_NAME", location.getCgiPath().c_str(), 1);
  setenv("PATH_INFO", request.getUri().c_str(), 1);
  setenv("PATH_TRANSLATED", request.getUri().c_str(), 1);

  setenv("QUERY_STRING", request.getArguments().c_str(), 1);

  std::string content_type = request.getHeader("Content-Type");
  if (!content_type.empty())
    setenv("CONTENT_TYPE", request.getHeader("Content-Type").c_str(), 1);

  std::string content_length = request.getHeader("Content-Type");
  if (!content_length.empty())
    setenv("CONTENT_LENGTH", request.getHeader("Content-Length").c_str(), 1);
}

static void setHttpMetaVariables(const webserv::RequestParser::headers &headers) {
  webserv::RequestParser::headers::const_iterator it;
  for (it = headers.begin(); it != headers.end(); it++) {
    std::string key = it->first;
    std::string value = it->second;

    std::transform(key.begin(), key.end(), key.begin(), toupper);
    std::replace(key.begin(), key.end(), '-', '_');

    setenv(("HTTP_" + key).c_str(), value.c_str(), 1);
  }
}

webserv::IResponse *webserv::generateCgiResponse(const RequestParser &request,
                                                 const ServerParser &server,
                                                 const LocationParser &location,
                                                 const t_client &client,
                                                 const std::string &cookie) {
  int body_fd;
  if (!request.getBodyPath().empty()) {
    body_fd = open(request.getBodyPath().c_str(), O_RDONLY);
    if (body_fd < 0)
      return (generateErrorResponse(500, server.getErrorPages(), request.getMethod() != HEAD));
    fcntl(body_fd, F_SETFL, O_NONBLOCK);
  } else {
    int empty_body[2];
    if (pipe(empty_body) != 0)
      return (generateErrorResponse(500, server.getErrorPages(), request.getMethod() != HEAD));
    body_fd = empty_body[0];
    close(empty_body[1]);
  }

  char output_path[] = "cgi_XXXXXX";
  int output_fd = mkstemp(output_path);
  if (output_fd < 0) {
    close(body_fd);
    return (generateErrorResponse(500, server.getErrorPages(), request.getMethod() != HEAD));
  }
  fcntl(output_fd, F_SETFL, O_NONBLOCK);

  const std::string &cgi_path = location.getCgiPath();
  std::string dir = cgi_path.substr(0, cgi_path.find_last_of('/'));
  std::string filename = cgi_path.substr(cgi_path.find_last_of('/') + 1);

  int pid = fork();
  int status;

  if (pid < 0) {
    close(body_fd);
    close(output_fd);
    std::remove(output_path);
    return (generateErrorResponse(500, server.getErrorPages(), request.getMethod()!=HEAD));
  }

  if (pid == 0) {
    dup2(body_fd, 0);
    dup2(output_fd, 1);

    chdir(dir.c_str());
    setCgiEnvironment(request, client, location);
    setHttpMetaVariables(request.getHeaders());

    execv(filename.c_str(), NULL);
    exit(1);
  } else {
    waitpid(pid, &status, 0);
    close(body_fd);
    close(output_fd);
  }

  if (WIFEXITED(status) == 0 || WEXITSTATUS(status) != 0) {
    std::remove(output_path);
    return generateErrorResponse(500, server.getErrorPages(), request.getMethod()!=HEAD);
  }

  return (new CgiResponse(output_path, cookie));
}
