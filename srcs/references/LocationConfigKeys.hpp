#ifndef WEBSERV_LOCATION_CONFIG_KEYS_HPP
#define WEBSERV_LOCATION_CONFIG_KEYS_HPP

#include <map>
#include <string>
#include <exception>
#include "references/configKeys.hpp"

namespace webserv {

class LocationConfigKeys {
  std::map<std::string, t_config_key> _keys;

  LocationConfigKeys();
  ~LocationConfigKeys();
  LocationConfigKeys(const LocationConfigKeys &src);
  LocationConfigKeys &operator=(const LocationConfigKeys &other);

 public:
  static LocationConfigKeys &getInstance();

  t_config_key find(const std::string &name) const;

  class KeyDoesNotExistsException : public std::exception {
   public:
    virtual const char *what() const throw() {
      return ("LocationConfigKeys: Key does not exists");
    }
  };
};

}

#endif // WEBSERV_LOCATION_CONFIG_KEYS_HPP
