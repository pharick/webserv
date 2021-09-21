#include "Response.hpp"

webserv::Response::Response(unsigned short status_code,
                            const headers &headers,
                            std::iostream *body)
    : _status(RESPONSE_HEADERS), _body(body) {
  _header << "HTTP/1.1 " << status_code << " " << Statuses::getInstance().getReasonPhrase(status_code) << "\r\n"
          << "Server: Sysoev" << "\r\n"
          << "Date: " << formatTime(time(NULL)) << "\r\n";

  headers::const_iterator it;
  for (it = headers.begin(); it != headers.end(); it++)
    _header << it->first << ": " << it->second << "\r\n";

  _header << "\r\n";
}

webserv::Response::~Response() {
  delete _body;
}

bool webserv::Response::sendChunk(int fd) {
  char buff[BUFF_LEN];
  ssize_t sent = 0;

  if (_status == RESPONSE_HEADERS) {
    if (_header.eof()) {
      _status = RESPONSE_BODY;
      return (true);
    }

    _header.read(buff, BUFF_LEN);
    sent = send(fd, buff, _header.gcount(), 0);
  } else if (_status == RESPONSE_BODY) {
    if (!_body || _body->eof()) {
      _status = RESPONSE_COMPLETE;
      return (true);
    }

    _body->read(buff, BUFF_LEN);
    sent = send(fd, buff, _body->gcount(), 0);
  }

  return (sent > 0);
}

bool webserv::Response::isComplete() const {
  return (_status == RESPONSE_COMPLETE);
}
