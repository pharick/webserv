#ifndef WEBSERV_CONNECTIONS_HPP
#define WEBSERV_CONNECTIONS_HPP

#include <iostream>
#include <set>
#include <map>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>

#define BACKLOG 256

namespace webserv {

typedef enum e_connection_status {
  READ,
  SEND
} t_connection_status;

typedef struct s_client {
  in_addr_t server_host;
  unsigned short server_port;
  in_addr_t client_host;
  unsigned short client_port;
  t_connection_status status;
} t_client;

class Connections {
 public:
  typedef std::set<int> servers;
  typedef std::map<int, t_client> clients;

 private:
  servers _server_sockets;
  clients _client_sockets;

  static struct sockaddr_in _getAddr(in_addr_t host, in_port_t port);

  Connections(const Connections &src);
  Connections &operator=(const Connections &other);

 public:
  Connections();
  ~Connections();

  void addServer(in_addr_t host, unsigned short port);
  int addClient(int server_fd);
  const t_client &getClientInfo(int fd) const;
  void setClientStatus(int fd, t_connection_status status);
  void disconnectClient(int fd);

  struct pollfd *getPollfdArray(size_t *servers_end, size_t *clients_end) const;

  class SocketErrorException : public std::exception {
   public:
    virtual const char *what() const throw() {
      return ("Connections: Socket error");
    }
  };

  class ClientDoesNotExistsException : public std::exception {
   public:
    virtual const char *what() const throw() {
      return ("Connections: Client does not exists");
    }
  };

  class CgiDoesNotExistsException : public std::exception {
   public:
    virtual const char *what() const throw() {
      return ("Connections: Cgi does not exists");
    }
  };
};

}

#endif // WEBSERV_CONNECTIONS_HPP
