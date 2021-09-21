#include "RequestParser.hpp"

webserv::RequestParser::RequestParser(const std::list<ServerParser *> &servers, t_client client)
  : _status(REQUEST_START_LINE), _client(client), _servers(servers), _server(NULL), _body_size(0) {}

webserv::RequestParser::~RequestParser() {
  std::remove(_body_path.c_str());
}

void webserv::RequestParser::addData(const char *data, size_t len) {
  _data.append(data, len);
  _parseData();
}

void webserv::RequestParser::_parseData() {
  if (_status == REQUEST_START_LINE && _isLine())
    _parseStartLine();

  while (_status == REQUEST_HEADERS && _isLine())
    _parseHeader();

  if ((_status == REQUEST_BODY_LENGTH || _status == REQUEST_BODY_CHUNKED_SIZE || _status == REQUEST_COMPLETE) && !_server) {
    _server = _findServer(_client.server_host, _client.server_port, getHeader("Host"));
    _location = _server->findLocation(getUri());

    _max_client_body_size = _location ? _location->getClientMaxBodySize() : 0;
    if (_max_client_body_size == 0)
      _max_client_body_size = _server->getClientMaxBodySize();
  }

  if (_status == REQUEST_BODY_LENGTH && _isNBytes(stringToUnsignedlong(getHeader("Content-Length"))))
    _parseBodyLength();

  while ((_status == REQUEST_BODY_CHUNKED_SIZE || _status == REQUEST_BODY_CHUNKED_DATA) && _isLine()) {
    if (_status == REQUEST_BODY_CHUNKED_SIZE)
      _parseBodyChunkedSize();
    else
      _parseBodyChunkedData();
  }

  if (_status == REQUEST_COMPLETE)
    _body_file.close();
}

bool webserv::RequestParser::_isLine() const {
  return (_data.find("\r\n") != std::string::npos);
}

std::string webserv::RequestParser::_getLine() {
  size_t pos = _data.find("\r\n");
  std::string line = _data.substr(0, pos);
  _data.erase(0, pos + 2);
  return (line);
}

bool webserv::RequestParser::_isNBytes(size_t n) const {
  return (_data.size() >= n);
}

std::string webserv::RequestParser::_getNBytes(size_t n) {
  std::string chunk = _data.substr(0, n);
  _data.erase(0, n);
  return (chunk);
}

void webserv::RequestParser::_parseStartLine() {
  std::string line = _getLine();

  if (line.empty())
    return;

  std::stringstream in(line);

  std::string method;
  in >> method;

  try {
    _method = Methods::getInstance().find(method);
  } catch (Methods::MethodDoesNotExistsException &e) {
    throw RequestParsingErrorException();
  }

  std::string uri;
  in >> uri;
  _parseUri(uri);

  std::string protocol;
  in >> protocol;
  if (protocol != "HTTP/1.1")
    throw RequestParsingErrorException();

  _status = REQUEST_HEADERS;
}

void webserv::RequestParser::_parseUri(const std::string &uri) {
  if (uri.empty() || uri[0] != '/')
    throw RequestParsingErrorException();

  size_t pos = uri.find('?');
  _uri = uri.substr(0, pos);

  if (pos != std::string::npos)
    _arguments = uri.substr(pos + 1);
}

void webserv::RequestParser::_parseHeader() {
  std::string line = _getLine();

  if (line.empty()) {
    std::string transfer_encoding = getHeader("Transfer-Encoding");
    size_t content_length = stringToUnsignedlong(getHeader("Content-Length"));

    if (transfer_encoding == "chunked") {
      _createBodyFile();
      _status = REQUEST_BODY_CHUNKED_SIZE;
    } else if (content_length > 0) {
      _createBodyFile();
      _status = REQUEST_BODY_LENGTH;
    } else {
      _status = REQUEST_COMPLETE;
    }
    return;
  }

  size_t colon_pos = line.find(':');

  if (colon_pos == std::string::npos)
    throw RequestParsingErrorException();

  std::string key = line.substr(0, colon_pos);
  std::string value = line.substr(colon_pos + 1);

  trim(key);
  trim(value);

  std::transform(key.begin(), key.end(), key.begin(), tolower);

  _headers[key] = value;
}

void webserv::RequestParser::_createBodyFile() {
  char body_path[] = "body_XXXXXX";
  int fd = mkstemp(body_path);
  close(fd);

  _body_path.assign(body_path);
  _body_file.open(_body_path, std::ios::trunc | std::ios::binary);
}

void webserv::RequestParser::_parseBodyLength() {
  size_t content_length = stringToUnsignedlong(getHeader("Content-Length"));

  if (content_length <= _max_client_body_size)
    _body_file << _getNBytes(stringToUnsignedlong(getHeader("Content-Length")));
  else
    _getNBytes(stringToUnsignedlong(getHeader("Content-Length")));

  _body_size = content_length;
  _status = REQUEST_COMPLETE;
}

void webserv::RequestParser::_parseBodyChunkedSize() {
  _body_chunked_size = stringHEXToUnsignedLong(_getLine());

  if (_body_chunked_size == 0)
    _status = REQUEST_COMPLETE;
  else
    _status = REQUEST_BODY_CHUNKED_DATA;
}

void webserv::RequestParser::_parseBodyChunkedData() {
  _body_size += _body_chunked_size;

  if (_body_size <= _max_client_body_size)
    _body_file << _getNBytes(_body_chunked_size);
  else
    _getNBytes(_body_chunked_size);

  std::string endline = _getLine();

  if (!endline.empty())
    throw RequestParsingErrorException();

  _status = REQUEST_BODY_CHUNKED_SIZE;
}

const webserv::ServerParser *webserv::RequestParser::_findServer(in_addr_t host, in_port_t port, const std::string &name) {
  std::list<ServerParser *>::const_iterator it;
  std::list<ServerParser *>::const_iterator first = _servers.end();

  for (it = _servers.begin(); it != _servers.end(); it++) {
    const ServerParser *server = *it;

    if (server->getHost() != host || server->getPort() != port)
      continue;

    if (first == _servers.end())
      first = it;

    if (name.empty() || server->getServerNames().find(name) != server->getServerNames().end())
      return (*it);
  }

  if (first == _servers.end())
    throw ServerDoesNotExistsException();

  return (*first);
}

webserv::t_method webserv::RequestParser::getMethod() const {
  return (_method);
}

const std::string &webserv::RequestParser::getUri() const {
  return (_uri);
}

const std::string &webserv::RequestParser::getArguments() const {
  return (_arguments);
}

const webserv::RequestParser::headers &webserv::RequestParser::getHeaders() const {
  return (_headers);
}

std::string webserv::RequestParser::getHeader(const std::string &key) const {
  std::string key_lower = key;
  std::transform(key_lower.begin(), key_lower.end(), key_lower.begin(), tolower);

  headers::const_iterator it = _headers.find(key_lower);

  if (it == _headers.end())
    return ("");

  return (it->second);
}

const std::string &webserv::RequestParser::getBodyPath() const {
  return (_body_path);
}

bool webserv::RequestParser::isBodyStart() const {
  return (_status == REQUEST_BODY_LENGTH || _status == REQUEST_BODY_CHUNKED_SIZE);
}

bool webserv::RequestParser::isComplete() const {
  return (_status == REQUEST_COMPLETE);
}

bool webserv::RequestParser::isBodyTooLarge() const {
  return (_body_size > _max_client_body_size);
}

const webserv::ServerParser *webserv::RequestParser::getServer() const {
  return (_server);
}

const webserv::LocationParser *webserv::RequestParser::getLocation() const {
  return (_location);
}

void webserv::RequestParser::print() const {
  std::cout << "Method: " << _method << std::endl
            << "URI: " << _uri << std::endl
            << "Arguments: " << _arguments << std::endl
            << "Headers:" << std::endl;

  for (headers::const_iterator it = _headers.begin(); it != _headers.end(); it++) {
    std::cout << it->first << ": " << it->second << std::endl;
  }

  std::cout << "Body path: " << _body_path << std::endl;
}

void webserv::RequestParser::reset() {
  _status = REQUEST_START_LINE;
  _data.clear();
  _uri.clear();
  _headers.clear();
  _max_client_body_size = 0;
  _body_size = 0;
  _server = NULL;
  _location = NULL;

  if (!_body_path.empty()) {
    std::remove(_body_path.c_str());
    _body_path.clear();
  }
}
