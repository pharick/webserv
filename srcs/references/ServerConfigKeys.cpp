#include "ServerConfigKeys.hpp"

webserv::ServerConfigKeys &webserv::ServerConfigKeys::getInstance() {
  static ServerConfigKeys ConfigKeys;
  return (ConfigKeys);
}

webserv::ServerConfigKeys::ServerConfigKeys() {
  _keys["host"] = HOST;
  _keys["port"] = PORT;
  _keys["server_names"] = SERVER_NAMES;
  _keys["error_page"] = ERROR_PAGES;
  _keys["client_max_body_size"] = CLIENT_MAX_BODY_SIZE;
}

webserv::ServerConfigKeys::~ServerConfigKeys() {}

webserv::t_config_key webserv::ServerConfigKeys::find(const std::string &name) const {
  std::map<std::string, t_config_key>::const_iterator it = _keys.find(name);

  if (it == _keys.end())
    throw KeyDoesNotExistsException();

  return (it->second);
}