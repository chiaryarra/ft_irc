#include "../../includes/server/Server.hpp"
#include <sys/socket.h>
#include <stdexcept>
#include <unistd.h>

Server::Server(int port, const std::string &password) {
    _port = port;
    _password = password;
    _serverSocketFd = -1;
    _running = false;
}

Server::~Server() {
    // Will need to have the close sockets fds, free memory and delete clients later
    if (_serverSocketFd >= 0)
        close(_serverSocketFd);
}

void    Server::setupSocket() {
    _serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocketFd < 0)
        throw std::runtime_error("Failed to create socket");
    int opt = 1;
    if (setsockopt(_serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("Failed to set socket options");
}

void    Server::start() {
    setupSocket();
    //setupPolling();
    //run();
}