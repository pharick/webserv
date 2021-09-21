#ifndef WEBSERV_CONFIG_PARSER_EXCEPTION_HPP
#define WEBSERV_CONFIG_PARSER_EXCEPTION_HPP

namespace webserv {

class ConfigParserErrorException : public std::exception {
 public:
  virtual const char *what() const throw() {
    return ("ConfigParser: Config parser error");
  }
};

class ServerDoesNotExistsException : public std::exception {
 public:
  virtual const char *what() const throw() {
    return ("ConfigParser: Server does not exists");
  }
};

}

#endif // WEBSERV_CONFIG_PARSER_EXCEPTION_HPP
