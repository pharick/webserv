#ifndef WEBSERV_METHODS_HPP
#define WEBSERV_METHODS_HPP

#include <map>
#include <string>
#include <exception>

namespace webserv {

typedef enum e_method {
  GET,
  HEAD,
  POST,
  PUT
} t_method;

class Methods {
  std::map<std::string, t_method> _str_to_tm;
  std::map<t_method, std::string> _tm_to_str;

  void _addMethod(const std::string &name, t_method key);

  Methods();
  ~Methods();
  Methods(const Methods &src);
  Methods &operator=(const Methods &other);

 public:
  static Methods &getInstance();

  t_method find(const std::string &name) const;
  const std::string &findName(t_method key) const;

  class MethodDoesNotExistsException : public std::exception {
   public:
    virtual const char *what() const throw() {
      return ("Methods: Method does not exists");
    }
  };
};

}

#endif // WEBSERV_METHODS_HPP
