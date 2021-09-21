#include "LocationConfigKeys.hpp"

webserv::LocationConfigKeys &webserv::LocationConfigKeys::getInstance() {
  static LocationConfigKeys ConfigKeys;
  return (ConfigKeys);
}

webserv::LocationConfigKeys::LocationConfigKeys() {
  _keys["methods"] = METHODS;
  _keys["redirect"] = REDIRECT;
  _keys["root"] = ROOT;
  _keys["autoindex"] = AUTOINDEX;
  _keys["index"] = INDEX;
  _keys["cgi"] = CGI;
  _keys["upload_store"] = UPLOAD_STORE;
  _keys["client_max_body_size"] = CLIENT_MAX_BODY_SIZE;
}

webserv::LocationConfigKeys::~LocationConfigKeys() {}

webserv::t_config_key webserv::LocationConfigKeys::find(const std::string &name) const {
  std::map<std::string, t_config_key>::const_iterator it = _keys.find(name);

  if (it == _keys.end())
    throw KeyDoesNotExistsException();

  return (it->second);
}
