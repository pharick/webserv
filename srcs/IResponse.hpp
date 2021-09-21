#ifndef WEBSERV_I_RESPONSE_HPP
#define WEBSERV_I_RESPONSE_HPP

namespace webserv {

class IResponse {
 public:
  virtual ~IResponse() {};
  virtual bool sendChunk(int fd) = 0;
  virtual bool isComplete() const = 0;
};

}

#endif // WEBSERV_I_RESPONSE_HPP
