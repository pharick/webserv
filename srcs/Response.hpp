#ifndef WEBSERV_RESPONSE_HEADER_HPP
#define WEBSERV_RESPONSE_HEADER_HPP

#include <iostream>
#include <sstream>
#include <ctime>
#include <sys/socket.h>
#include "references/Statuses.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "IResponse.hpp"

namespace webserv {

typedef enum e_response_status {
  RESPONSE_HEADERS,
  RESPONSE_BODY,
  RESPONSE_COMPLETE
} t_response_status;

class Response : public IResponse {
 public:
  typedef std::map<std::string, std::string> headers;

 private:
  t_response_status _status;
  std::stringstream _header;
  std::iostream *_body;

  Response();
  Response(const Response &src);
  Response &operator=(const Response &other);

 public:
  Response(unsigned short status_code,
           const headers &headers,
           std::iostream *body);
  ~Response();

  bool sendChunk(int fd);
  bool isComplete() const;
};

}

#endif // WEBSERV_RESPONSE_HEADER_HPP
