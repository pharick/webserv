#include "configParser.hpp"

/**
 * Parse config and error checking
 * @param path Path to the config file
 * @return List of virtual servers
 */
std::list<webserv::ServerParser *> webserv::parseConfig(const std::string &path) {
  std::ifstream fin(path);
  std::list<ServerParser *> servers;
  std::set<std::string> names;

  if (!fin)
    throw ConfigParserErrorException();

  std::string line;
  std::string token;
  while (getline(fin, line)) {
    if (line.empty())
      continue;

    std::stringstream line_s(line);

    line_s >> token;
    if (token != "server")
      throw ConfigParserErrorException();

    line_s >> token;
    if (token != "{" || !line_s.eof())
      throw ConfigParserErrorException();

    ServerParser *server = new ServerParser(fin);

    ServerParser::server_names server_names = server->getServerNames();
    ServerParser::server_names::iterator it;
    for (it = server_names.begin(); it != server_names.end(); it++) {
      if (names.find(*it) == names.end())
        names.insert(*it);
      else
        throw ConfigParserErrorException();
    }
    servers.push_back(server);
  }

  return (servers);
}

void webserv::printConfig(const std::list<webserv::ServerParser *> &servers) {
  std::list<webserv::ServerParser *>::const_iterator servers_it;
  for (servers_it = servers.begin(); servers_it != servers.end(); servers_it++) {
    (*servers_it)->print();
    std::cout << std::endl;
  }
}
