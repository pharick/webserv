#ifndef WEBSERV_CGI_RESPONSE_HPP
#define WEBSERV_CGI_RESPONSE_HPP

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <sys/socket.h>
#include "defines.hpp"
#include "IResponse.hpp"
#include "utils.hpp"

namespace webserv {

typedef enum e_cgi_response_status {
  CGI_RESPONSE_HEADERS,
  CGI_RESPONSE_CGI_HEADERS,
  CGI_RESPONSE_BODY,
  CGI_RESPONSE_COMPLETE
} t_cgi_response_status;

class CgiResponse : public IResponse {
  t_cgi_response_status _status;
  std::stringstream _header;
  std::string _cgi_output_path;
  std::iostream *_cgi_output;

  CgiResponse();
  CgiResponse(const CgiResponse &src);
  CgiResponse &operator=(const CgiResponse &other);

 public:
  explicit CgiResponse(const std::string &cgi_output_path, const std::string &cookie = std::string());
  ~CgiResponse();

  bool sendChunk(int fd);
  bool isComplete() const;
};

}

#endif // WEBSERV_CGI_RESPONSE_HPP
