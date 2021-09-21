#ifndef WEBSERV_SERVER_CONFIG_KEYS_HPP
#define WEBSERV_SERVER_CONFIG_KEYS_HPP

#include <map>
#include <string>
#include <exception>
#include "references/configKeys.hpp"

namespace webserv {

class ServerConfigKeys {
  std::map<std::string, t_config_key> _keys;

  ServerConfigKeys();
  ~ServerConfigKeys();
  ServerConfigKeys(const ServerConfigKeys &src);
  ServerConfigKeys &operator=(const ServerConfigKeys &other);

 public:
  static ServerConfigKeys &getInstance();

  t_config_key find(const std::string &name) const;

  class KeyDoesNotExistsException : public std::exception {
   public:
    virtual const char *what() const throw() {
      return ("ServerConfigKeys: Key does not exists");
    }
  };
};

}

#endif // WEBSERV_SERVER_CONFIG_KEYS_HPP