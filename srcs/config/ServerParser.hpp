#ifndef WEBSERV_SERVER_PARSER_HPP
#define WEBSERV_SERVER_PARSER_HPP

#include <iostream>
#include <fstream>
#include <set>
#include <list>
#include <arpa/inet.h>
#include <climits>
#include "configParserExceptions.hpp"
#include "config/LocationParser.hpp"
#include "references/ServerConfigKeys.hpp"
#include "references/Statuses.hpp"
#include "utils.hpp"

namespace webserv {

class ServerParser {
 public:
  typedef std::set<std::string> server_names;
  typedef std::map<unsigned short, std::string> error_pages;
  typedef std::map<std::string, LocationParser *> locations_prefix;
  typedef std::map<std::string, LocationParser *> locations_extension;
  typedef std::set<t_config_key> present_keys;

 private:
  in_addr_t _host;
  in_port_t _port;
  server_names _server_names;
  error_pages _error_pages;
  size_t _client_max_body_size;
  locations_prefix _locations_prefix;
  locations_extension _locations_extension;
  present_keys _present_keys;

  void _parseParam(const std::string &str);
  void _parseHost(const std::string &str);
  void _parsePort(const std::string &str);
  void _parseServerNames(const std::string &str);
  void _parseErrorPages(const std::string &str);
  void _parseClientMaxBodySize(const std::string &str);
  void _parseLocation(std::stringstream &line, std::ifstream &fin);

  ServerParser();
  ServerParser(const ServerParser &src);
  ServerParser &operator=(const ServerParser &other);

 public:
  explicit ServerParser(std::ifstream &fin);
  ~ServerParser();

  in_addr_t getHost() const;
  in_port_t getPort() const;
  const server_names &getServerNames() const;
  const error_pages &getErrorPages() const;
  size_t getClientMaxBodySize() const;

  LocationParser *findLocation(const std::string &uri) const;

  void print() const;
};

}

#endif // WEBSERV_SERVER_PARSER_HPP
