#include "ServerParser.hpp"

webserv::ServerParser::ServerParser(std::ifstream &fin) : _host(0), _port(0), _client_max_body_size(10000000) {
  std::string line;

  std::getline(fin, line);

  while (trim(line) != "}") {
    if (line.empty()) {
      std::getline(fin, line);
      continue;
    }

    std::stringstream line_s(line);

    std::string token;
    line_s >> token;

    if (token == "location") {
      _parseLocation(line_s, fin);
      std::getline(fin, line);
      continue;
    }

    size_t p = line.find(';');

    if (p == std::string::npos)
      throw ConfigParserErrorException();

    line.erase(p);
    trim(line);
    if (line.empty()) {
      std::getline(fin, line);
      continue;
    }

    _parseParam(line);
    std::getline(fin, line);
  }
}

webserv::ServerParser::~ServerParser() {}

void webserv::ServerParser::_parseParam(const std::string &str) {
  std::string key_token;
  std::string value;
  std::stringstream in(str);

  in >> key_token;

  t_config_key key;
  try {
    key = ServerConfigKeys::getInstance().find(key_token);
  } catch (ServerConfigKeys::KeyDoesNotExistsException &e) {
    throw ConfigParserErrorException();
  }

  if (key != ERROR_PAGES && _present_keys.find(key) != _present_keys.end())
    throw ConfigParserErrorException();

  std::getline(in, value);
  trim(value);

  if (value.empty())
    throw ConfigParserErrorException();

  if (key == HOST)
    _parseHost(value);
  else if (key == PORT)
    _parsePort(value);
  else if (key == SERVER_NAMES)
    _parseServerNames(value);
  else if (key == ERROR_PAGES)
    _parseErrorPages(value);
  else if (key == CLIENT_MAX_BODY_SIZE)
    _parseClientMaxBodySize(value);
  else
    throw ConfigParserErrorException();

  _present_keys.insert(key);
}

void webserv::ServerParser::_parseHost(const std::string &str) {
  unsigned int host = inet_addr(str.c_str());

  if (host >= UINT_MAX)
    throw ConfigParserErrorException();

  _host = host;
}

void webserv::ServerParser::_parsePort(const std::string &str) {
  std::stringstream in(str);
  in_port_t port;

  in >> port;
  if (in.fail() || !in.eof())
    throw ConfigParserErrorException();

  _port = port;
}

void webserv::ServerParser::_parseServerNames(const std::string &str) {
  std::stringstream in(str);
  std::string value;

  if (!_server_names.empty())
    throw ConfigParserErrorException();

  while (!in.eof()) {
    in >> value;
    if (_server_names.find(value) != _server_names.end())
      throw ConfigParserErrorException();

    _server_names.insert(value);
  }
}

void webserv::ServerParser::_parseErrorPages(const std::string &str) {
  unsigned short key;
  std::string str_copy = str;

  size_t last_space = str_copy.find_last_of(" \t\r\n\v\f");
  if (last_space == std::string::npos)
    throw ConfigParserErrorException();

  std::string value = str_copy.substr(last_space + 1);
  str_copy.erase(last_space + 1);
  trim(str_copy);

  std::stringstream in(str_copy);

  while (!in.eof()) {
    in >> key;
    if (in.fail())
      throw ConfigParserErrorException();

    Statuses::getInstance().getReasonPhrase(key);

    if (_error_pages.find(key) != _error_pages.end())
      throw ConfigParserErrorException();

    _error_pages[key] = value;
  }
}

void webserv::ServerParser::_parseClientMaxBodySize(const std::string &str) {
  std::stringstream in(str);
  size_t client_max_body_size;

  in >> client_max_body_size;
  if (in.fail() || !in.eof())
    throw ConfigParserErrorException();

  _client_max_body_size = client_max_body_size;
}

void webserv::ServerParser::_parseLocation(std::stringstream &line, std::ifstream &fin) {
  std::string token;

  line >> token;
  if (token == "*") {
    std::string extension;
    line >> extension;

    if (extension == "{")
      throw ConfigParserErrorException();

    while (extension != "{") {
      if (_locations_extension.find(extension) != _locations_extension.end()) {
        throw ConfigParserErrorException();
      }

      _locations_extension[extension] = NULL;
      line >> extension;
    }

    LocationParser *location = new LocationParser("", fin);

    locations_extension::iterator it;
    for (it = _locations_extension.begin(); it != _locations_extension.end(); it++) {
      if (!it->second)
        it->second = location;
    }

    return;
  }

  std::string uri = token;

  if (uri.size() > 1 && uri.back() == '/')
    uri.erase(uri.size() - 1);

  if (_locations_prefix.find(uri) != _locations_prefix.end())
    throw ConfigParserErrorException();

  line >> token;

  if (token != "{" || !line.eof() || uri[0] != '/')
    throw ConfigParserErrorException();

  _locations_prefix[uri] = new LocationParser(uri, fin);
}

in_addr_t webserv::ServerParser::getHost() const {
  return (_host);
}

in_port_t webserv::ServerParser::getPort() const {
  return (_port);
}

const webserv::ServerParser::server_names &webserv::ServerParser::getServerNames() const {
  return (_server_names);
}

const webserv::ServerParser::error_pages &webserv::ServerParser::getErrorPages() const {
  return (_error_pages);
}

size_t webserv::ServerParser::getClientMaxBodySize() const {
  return (_client_max_body_size);
}

webserv::LocationParser *webserv::ServerParser::findLocation(const std::string &uri) const {
  std::string extension = uri.substr(uri.find_last_of('.') + 1);
  locations_extension::const_iterator it_ext = _locations_extension.find(extension);
  if (it_ext != _locations_extension.end())
    return (it_ext->second);

  locations_prefix::const_iterator res = _locations_prefix.end();
  size_t max_len = 0;

  locations_prefix::const_iterator it;
  for (it = _locations_prefix.begin(); it != _locations_prefix.end(); it++) {
    std::string location_uri = it->first;
    LocationParser *location = it->second;

    if (uri == location_uri || uri == location_uri + "/")
      return (it->second);

    if (location->getRedirect().empty() && uri.substr(0, location_uri.size()) == location_uri && location_uri.size() > max_len) {
      res = it;
      max_len = location_uri.size();
    }
  }

  return (res != _locations_prefix.end() ? res->second : NULL);
}

void webserv::ServerParser::print() const {
  std::cout << "host: " << _host << std::endl
            << "port: " << _port << std::endl;

  std::cout << "server_names: ";
  server_names::const_iterator names_it;
  for (names_it = _server_names.begin(); names_it != _server_names.end(); names_it++) {
    std::cout << *names_it << " ";
  }
  std::cout << std::endl;

  std::cout << "error_pages: ";
  error_pages::const_iterator error_pages_it;
  for (error_pages_it = _error_pages.begin(); error_pages_it != _error_pages.end(); error_pages_it++) {
    std::cout << error_pages_it->first << ": " << error_pages_it->second << " ";
  }
  std::cout << std::endl;

  std::cout << "client_max_body_size: " << _client_max_body_size << std::endl;

  std::cout << "LOCATIONS:" << std::endl;
  locations_prefix::const_iterator locations_it;
  for (locations_it = _locations_prefix.begin(); locations_it != _locations_prefix.end(); locations_it++) {
    std::cout << "uri: " << locations_it->first << std::endl;
    (*locations_it->second).print();
    std::cout << std::endl;
  }
}
