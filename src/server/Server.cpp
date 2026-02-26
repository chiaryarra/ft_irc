#include "../../includes/server/Server.hpp"
#include <sys/socket.h>
#include <stdexcept>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <cstring>
#include <arpa/inet.h>

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

void    Server::setupSocketOpts() {
    // Setup the options that our server socket will have
    int opt = 1;
    if (setsockopt(_serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw   std::runtime_error("Failed to set socket options");
}

void    Server::setupServerAddress() {
    // Setup the address and port that our socket will be listening
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = INADDR_ANY;
    std::memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
    if (bind(_serverSocketFd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        throw   std::runtime_error("Failed to bind socket port and address");
}

void    Server::setupListen() {
    if (listen(_serverSocketFd, SOMAXCONN) < 0)
        throw   std::runtime_error("Failed to make the socket listen");
}

void    Server::setupSocket() {
    _serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocketFd < 0)
        throw   std::runtime_error("Failed to create socket");
    setupSocketOpts();
    setupServerAddress();
    setupListen();
}

void    Server::start() {
    setupSocket();
    //setupPolling();
    //run();
}