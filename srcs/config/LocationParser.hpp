#ifndef WEBSERV_LOCATION_PARSER_HPP
#define WEBSERV_LOCATION_PARSER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <list>
#include <sys/stat.h>
#include "configParserExceptions.hpp"
#include "references/LocationConfigKeys.hpp"
#include "references/Methods.hpp"
#include "utils.hpp"

namespace webserv {

class LocationParser {
 public:
  typedef std::set<t_method> methods;
  typedef std::list<std::string> index_pages;
  typedef std::set<t_config_key> present_keys;

 private:
  std::string _uri;
  methods _methods;
  std::string _redirect;
  std::string _root;
  bool _autoindex;
  index_pages _index_pages;
  std::string _upload_store;
  size_t _client_max_body_size;
  std::string _cgi_path;
  present_keys _present_keys;

  void _parseParam(const std::string &str);
  void _parseMethods(const std::string &str);
  void _parseRedirect(const std::string &str);
  void _parseRoot(const std::string &str);
  void _parseAutoindex(const std::string &str);
  void _parseIndexPages(const std::string &str);
  void _parseCgi(const std::string &str);
  void _parseUploadStore(const std::string &str);
  void _parseClientMaxBodySize(const std::string &str);

  LocationParser();
  LocationParser(const LocationParser &src);
  LocationParser &operator=(const LocationParser &other);

 public:
  explicit LocationParser(const std::string &uri, std::ifstream &fin);
  ~LocationParser();

  const std::string &getUri() const;
  const methods &getMethods() const;
  const std::string &getRoot() const;
  const index_pages &getIndexPages() const;
  bool isAutoindex() const;
  const std::string &getUploadStore() const;
  const std::string &getRedirect() const;
  const std::string &getCgiPath() const;
  size_t getClientMaxBodySize() const;

  void print() const;
};

}

#endif // WEBSERV_LOCATION_PARSER_HPP
