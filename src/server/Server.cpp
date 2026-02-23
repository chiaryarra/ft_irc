#include "../../includes/server/Server.hpp"

Server::Server(int port, const std::string &password) {
    _port = port;
    _password = password;
    _serverSocketFd = 0;
    _running = false;
}

Server::~Server() {
    // Will need to have the close sockets fds, free memory and delete clients later
}

void    Server::start() {
    //setupSocket();
    //setupPolling();
    //run();
}