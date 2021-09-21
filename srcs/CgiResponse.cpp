#include "CgiResponse.hpp"

webserv::CgiResponse::CgiResponse(const std::string &cgi_output_path, const std::string &cookie)
    : _status(CGI_RESPONSE_HEADERS), _cgi_output_path(cgi_output_path) {
  _cgi_output = new std::fstream(cgi_output_path);

  _header << "HTTP/1.1 200 OK" << "\r\n"
          << "Server: Sysoev" << "\r\n"
          << "Date: " << formatTime(time(NULL)) << "\r\n";

  _header << "Transfer-Encoding: chunked\r\n";

  if (!cookie.empty())
    _header << "Set-Cookie: " << cookie << "\r\n";
}

webserv::CgiResponse::~CgiResponse() {
  delete _cgi_output;
  std::remove(_cgi_output_path.c_str());
}

bool webserv::CgiResponse::sendChunk(int fd) {
  char buff[BUFF_LEN];
  ssize_t sent = 0;

  if (_status == CGI_RESPONSE_HEADERS) {
    if (_header.eof()) {
      _status = CGI_RESPONSE_CGI_HEADERS;
      return (true);
    }

    _header.read(buff, BUFF_LEN);
    sent = send(fd, buff, _header.gcount(), 0);
  } else if (_status == CGI_RESPONSE_CGI_HEADERS) {
    std::string header;
    getline(*_cgi_output, header);

    if (header!="\r" && !header.empty()) {
      sent = send(fd, (header + "\n").c_str(), header.size() + 1, 0);
    } else {
      sent = send(fd, "\r\n", 2, 0);
      _status = CGI_RESPONSE_BODY;
    }
  } else if (_status == CGI_RESPONSE_BODY) {
    if (_cgi_output->eof()) {
      sent = send(fd, "0\r\n\r\n", 5, 0);
      _status = CGI_RESPONSE_COMPLETE;
    } else {
      _cgi_output->read(buff, BUFF_LEN);
      buff[_cgi_output->gcount()] = '\0';
      std::string hex = unsignedLongToHEXString(_cgi_output->gcount());

      size_t len = hex.size() + 2 + _cgi_output->gcount() + 2 + 1;
      char *chunk = new char [len];
      strlcpy(chunk, hex.c_str(), len);
      strlcat(chunk, "\r\n", len);
      strlcat(chunk, buff, len);
      strlcat(chunk, "\r\n", len);

      sent = send(fd, chunk, len - 1, 0);

      delete [] chunk;
    }
  }

  return (sent > 0);
}

bool webserv::CgiResponse::isComplete() const {
  return (_status == CGI_RESPONSE_COMPLETE);
}
