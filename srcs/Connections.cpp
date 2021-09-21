#include "Connections.hpp"

webserv::Connections::Connections() {}

webserv::Connections::~Connections() {}

void webserv::Connections::addServer(in_addr_t host, unsigned short port) {
  int sock = socket(PF_INET, SOCK_STREAM, 0);
  fcntl(sock, F_SETFL, O_NONBLOCK);
  if (sock < 0)
    throw SocketErrorException();

  int enable = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
    throw SocketErrorException();

  struct sockaddr_in addr = _getAddr(host, port);

  if (bind(sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0)
    throw SocketErrorException();

  if (listen(sock, BACKLOG) < 0)
    throw SocketErrorException();

  _server_sockets.insert(sock);
}

int webserv::Connections::addClient(int server_fd) {
  struct sockaddr_in server_addr = {};
  socklen_t server_addr_size = sizeof(server_addr);
  getsockname(server_fd, reinterpret_cast<sockaddr *>(&server_addr), &server_addr_size);

  struct sockaddr_in client_addr = {};
  socklen_t client_addr_size = sizeof(client_addr);
  int client_socket = accept(server_fd, reinterpret_cast<struct sockaddr *>(&client_addr), &client_addr_size);
  fcntl(client_socket, F_SETFL, O_NONBLOCK);

  t_client client;
  client.server_host = server_addr.sin_addr.s_addr;
  client.server_port = ntohs(server_addr.sin_port);
  client.client_host = client_addr.sin_addr.s_addr;
  client.client_port = ntohs(client_addr.sin_port);
  client.status = READ;

  _client_sockets[client_socket] = client;

  std::cout << "New connection at host " << inet_ntoa(*reinterpret_cast<const in_addr *>(&client.server_host))
            << " port " << client.server_port << "; "
            << "Client host " << inet_ntoa(*reinterpret_cast<const in_addr *>(&client.client_host))
            << " port " << client.client_port << std::endl;

  return (client_socket);
}

struct sockaddr_in webserv::Connections::_getAddr(in_addr_t host, in_port_t port) {
  struct sockaddr_in sockaddr = {};
  sockaddr.sin_family = PF_INET;
  sockaddr.sin_addr.s_addr = host;
  sockaddr.sin_port = htons(port);
  return sockaddr;
}

struct pollfd *webserv::Connections::getPollfdArray(size_t *servers_end, size_t *clients_end) const {
  *servers_end = _server_sockets.size();
  *clients_end =  *servers_end + _client_sockets.size();
  struct pollfd *fds = new struct pollfd [*clients_end];

  size_t i = 0;

  std::set<int>::const_iterator server_it;
  for (server_it = _server_sockets.begin(); server_it != _server_sockets.end(); server_it++) {
    fds[i].fd = *server_it;
    fds[i].events = POLLIN;
    fds[i].revents = 0;
    i++;
  }

  clients::const_iterator client_it;
  for (client_it = _client_sockets.begin(); client_it != _client_sockets.end(); client_it++) {
    fds[i].fd = client_it->first;
    fds[i].events = 0;

    if (client_it->second.status == READ)
      fds[i].events = POLLIN;
    else if (client_it->second.status == SEND)
      fds[i].events = POLLOUT;

    fds[i].revents = 0;
    i++;
  }

  return (fds);
}

const webserv::t_client &webserv::Connections::getClientInfo(int fd) const {
  if (_client_sockets.find(fd) == _client_sockets.end())
    throw ClientDoesNotExistsException();

  return (_client_sockets.find(fd)->second);
}

void webserv::Connections::setClientStatus(int fd, t_connection_status status) {
  _client_sockets[fd].status = status;
}

void webserv::Connections::disconnectClient(int fd) {
  if (_client_sockets.find(fd) == _client_sockets.end())
    throw ClientDoesNotExistsException();

  t_client client = _client_sockets[fd];

  std::cout << "Disconnect client from host " << inet_ntoa(*reinterpret_cast<const in_addr *>(&client.server_host))
            << " port " << client.server_port << "; "
            << "Client host " << inet_ntoa(*reinterpret_cast<const in_addr *>(&client.client_host))
            << " port " << client.client_port << std::endl;

  close(fd);
  _client_sockets.erase(fd);
}
