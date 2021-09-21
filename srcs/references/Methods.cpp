#include "Methods.hpp"

webserv::Methods &webserv::Methods::getInstance() {
  static Methods methods;
  return (methods);
}

webserv::Methods::Methods() {
  _addMethod("GET", GET);
  _addMethod("HEAD", HEAD);
  _addMethod("POST", POST);
  _addMethod("PUT", PUT);
}

void webserv::Methods::_addMethod(const std::string &name, webserv::t_method key) {
  _str_to_tm[name] = key;
  _tm_to_str[key] = name;
}

webserv::Methods::~Methods() {}

webserv::t_method webserv::Methods::find(const std::string &name) const {
  std::map<std::string, t_method>::const_iterator it = _str_to_tm.find(name);

  if (it == _str_to_tm.end())
    throw MethodDoesNotExistsException();

  return (it->second);
}

const std::string &webserv::Methods::findName(webserv::t_method key) const {
  std::map<t_method, std::string>::const_iterator it = _tm_to_str.find(key);

  if (it == _tm_to_str.end())
    throw MethodDoesNotExistsException();

  return (it->second);
}
