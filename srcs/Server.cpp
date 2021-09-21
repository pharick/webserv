#include "Server.hpp"

webserv::Request::Request(const std::list<ServerParser *> &servers, t_client client) : response(NULL), disconnect(false) {
  parser = new RequestParser(servers, client);
}

webserv::Request::~Request() {
  delete parser;
  delete response;
}

void webserv::Request::reset() {
  delete response;
  response = NULL;
  disconnect = false;
  parser->reset();
}

webserv::Server::Server(const std::list<ServerParser *> &servers) : _servers(servers) {
  std::set<std::pair<in_addr_t, in_port_t> > addresses = _getServerAddresses();

  std::set<std::pair<in_addr_t, in_port_t> >::const_iterator it;
  for (it = addresses.begin(); it != addresses.end(); it++) {
    in_addr_t host = it->first;
    in_port_t port = it->second;

    _connections.addServer(host, port);

    std::cout << "Server listen at host " << inet_ntoa(*reinterpret_cast<const in_addr *>(&host))
              << " port " << port << std::endl;
  }
}

webserv::Server::~Server() {}

void webserv::Server::poll() {
  while (true) {
    size_t servers_end;
    size_t clients_end;
    struct pollfd *fds = _connections.getPollfdArray(&servers_end, &clients_end);

    int ready = ::poll(fds, clients_end, -1);

    if (ready < 0)
      throw std::exception();

    if (ready > 0)
      _processPollfd(fds, servers_end, clients_end);

    delete[] fds;
  }
}

std::set<std::pair<in_addr_t, in_port_t> > webserv::Server::_getServerAddresses() const {
  std::set<std::pair<in_addr_t, in_port_t> > addresses;

  std::list<ServerParser *>::const_iterator server_it;
  for (server_it = _servers.begin(); server_it != _servers.end(); server_it++) {
    addresses.insert(std::make_pair((*server_it)->getHost(), (*server_it)->getPort()));
  }

  return (addresses);
}

void webserv::Server::_processPollfd(struct pollfd *fds, size_t servers_end, size_t clients_end) {
  for (size_t i = 0; i < clients_end; i++) {
    int fd = fds[i].fd;

    if (i < servers_end) {
      if (fds[i].revents & POLLIN) {
        int client_fd = _connections.addClient(fds[i].fd);
        _requests[client_fd] = new Request(_servers, _connections.getClientInfo(client_fd));
      }
    } else if (i < clients_end) {
      if (fds[i].revents & POLLIN)
        _readClientData(fd);
      else if (fds[i].revents & POLLOUT)
        _sendResponseData(fd);
    }
  }
}

void webserv::Server::_readClientData(int fd) {
  char buff[BUFF_LEN];
  ssize_t ret;

  ret = recv(fd, buff, BUFF_LEN, 0);

  if (ret <= 0)
    return (_disconnectClient(fd));

  try {
    _requests[fd]->parser->addData(buff, ret);
  } catch (RequestParser::RequestParsingErrorException &e) {
    _requests[fd]->response =
        generateErrorResponse(400, ServerParser::error_pages(), false);
    _requests[fd]->disconnect = true;
    _connections.setClientStatus(fd, SEND);
    return;
  }

  if (_requests[fd]->parser->isComplete())
    _processRequest(fd);
}

void webserv::Server::_processRequest(int fd) {
  t_client client = _connections.getClientInfo(fd);
  const RequestParser &request = *_requests[fd]->parser;

  const ServerParser &server = *request.getServer();
  const LocationParser *location = request.getLocation();

  std::string cookie = _requests[fd]->parser->getHeader("Cookie");
  std::string new_cookie;

  std::cout << "New request at host " << inet_ntoa(*reinterpret_cast<const in_addr *>(&client.server_host))
            << " port " << client.server_port << "; "
            << "Client host " << inet_ntoa(*reinterpret_cast<const in_addr *>(&client.client_host))
            << " port " << client.client_port;

  if (!cookie.empty()) {
    std::cout << "; Cookie: " << cookie;
  } else {
    new_cookie = _generateCookie(client);
    std::cout << "; Set-Cookie: " << new_cookie;
  }

  std::cout << std::endl;

  if (!location) {
    _requests[fd]->response = generateErrorResponse(404, server.getErrorPages(), request.getMethod() != HEAD, new_cookie);
    _connections.setClientStatus(fd, SEND);
    return;
  }

  if (!location->getRedirect().empty()) {
    _requests[fd]->response = generateRedirectResponse(*location, new_cookie);
    _connections.setClientStatus(fd, SEND);
    return;
  }

  const LocationParser::methods &methods = location->getMethods();

  if (methods.find(request.getMethod()) == methods.end()) {
    _requests[fd]->response = generateErrorResponse(405, server.getErrorPages(), request.getMethod() != HEAD, new_cookie);
    _connections.setClientStatus(fd, SEND);
    return;
  }

  if (request.isBodyTooLarge()) {
    _requests[fd]->response = generateErrorResponse(413, server.getErrorPages(), request.getMethod() != HEAD, new_cookie);
    _connections.setClientStatus(fd, SEND);
    return;
  }

  if (!location->getCgiPath().empty()) {
    _requests[fd]->response = generateCgiResponse(request, server, *location, client, new_cookie);
    _connections.setClientStatus(fd, SEND);
    return;
  }

  if (request.getMethod() == GET)
    _requests[fd]->response = generateStaticResponse(request, server, *location, request.getUri(), new_cookie);
  else if (request.getMethod() == PUT)
    _requests[fd]->response = generatePutResponse(request, *location, new_cookie);

  _connections.setClientStatus(fd, SEND);
}

void webserv::Server::_sendResponseData(int fd) {
  if (!_requests[fd]->response)
    return;

  IResponse *response = _requests[fd]->response;

  if (response->isComplete()) {
    if (_requests[fd]->disconnect) {
      _disconnectClient(fd);
      return;
    }

    _requests[fd]->reset();
    _connections.setClientStatus(fd, READ);
    return;
  }

  if (!response->sendChunk(fd)) {
    std::cout << "here" << std::endl;
    _disconnectClient(fd);
  }
}

std::string webserv::Server::_generateCookie(const t_client &client) {
  time_t t;
  time(&t);
  return (unsignedLongToHEXString(client.client_host + client.server_port + t));
}

void webserv::Server::_disconnectClient(int fd) {
  delete _requests[fd];
  _requests.erase(fd);
  _connections.disconnectClient(fd);
}
