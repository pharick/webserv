#ifndef WEBSERV_REQUEST_PARSER_HPP
#define WEBSERV_REQUEST_PARSER_HPP

#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <cctype>
#include <fstream>
#include <unistd.h>
#include "Connections.hpp"
#include "config/ServerParser.hpp"
#include "references/Methods.hpp"
#include "utils.hpp"

namespace webserv {

typedef enum e_parser_status {
  REQUEST_START_LINE,
  REQUEST_HEADERS,
  REQUEST_BODY_LENGTH,
  REQUEST_BODY_CHUNKED_SIZE,
  REQUEST_BODY_CHUNKED_DATA,
  REQUEST_COMPLETE
} t_parser_status;

class RequestParser {
 public:
  typedef std::list<ServerParser *> servers;
  typedef std::map<std::string, std::string> headers;

 private:
  t_parser_status _status;
  std::string _data;

  t_client _client;
  const servers &_servers;
  const ServerParser *_server;
  const LocationParser *_location;

  t_method _method;
  std::string _uri;
  std::string _arguments;
  headers _headers;
  size_t _max_client_body_size;
  size_t _body_chunked_size;
  size_t _body_size;
  std::string _body_path;
  std::ofstream _body_file;

  bool _isLine() const;
  std::string _getLine();
  bool _isNBytes(size_t n) const;
  std::string _getNBytes(size_t n);

  void _parseData();
  void _parseStartLine();
  void _parseHeader();
  void _parseUri(const std::string &uri);
  void _createBodyFile();
  void _parseBodyLength();
  void _parseBodyChunkedSize();
  void _parseBodyChunkedData();
  const ServerParser *_findServer(in_addr_t host, in_port_t port, const std::string &name);

  RequestParser();
  RequestParser(const RequestParser &src);
  RequestParser &operator=(const RequestParser &other);

 public:
  RequestParser(const std::list<ServerParser *> &servers, t_client client);
  ~RequestParser();

  void addData(const char *data, size_t len);

  t_method getMethod() const;
  const std::string &getUri() const;
  const std::string &getArguments() const;
  const headers &getHeaders() const;
  std::string getHeader(const std::string &key) const;
  const std::string &getBodyPath() const;
  const ServerParser *getServer() const;
  const LocationParser *getLocation() const;

  bool isBodyStart() const;
  bool isComplete() const;
  bool isBodyTooLarge() const;
  void print() const;

  void reset();

  class RequestParsingErrorException : public std::exception {
   public:
    virtual const char *what() const throw() {
      return ("RequestParser: Request parsing error");
    }
  };
};

}

#endif // WEBSERV_REQUEST_PARSER_HPP
