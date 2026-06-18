#include "../../includes/server/Server.hpp"
#include "../../includes/client/Client.hpp"
#include "../../includes/utils/Utils.hpp"
#include <sstream>
#include <sys/socket.h>
#include <stdexcept>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <cstring>
#include <arpa/inet.h>
#include <iostream>
#include <cerrno>
#include <set>

Server::Server(int port, const std::string &password) {
    _port = port;
    _password = password;
    _serverSocketFd = -1;
    _running = false;
	initCommandMap();
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

void    Server::setupNonBlocking() {
    int flags = fcntl(_serverSocketFd, F_GETFL, 0);
    if (flags < 0)
        throw   std::runtime_error("Failed to get socket fd flags");
    if (fcntl(_serverSocketFd, F_SETFL,  flags | O_NONBLOCK) < 0)
        throw   std::runtime_error("Failed to set Non blocking on socket fd");
}

void    Server::setupServerPoll() {
    pollfd  serverPoll;

    serverPoll.fd = _serverSocketFd;
    serverPoll.events = POLLIN;
    serverPoll.revents = 0;
    _pollFds.push_back(serverPoll);
}

void    Server::setupPolling() {
    setupNonBlocking();
    setupServerPoll();
}

void    Server::handleNewConnection() {
    while (true)
    {
        int clientFd = accept(_serverSocketFd, NULL, NULL);
        if (clientFd < 0)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
                break;
            else
                throw std::runtime_error("Accept execution failed");
        }
        fcntl(clientFd, F_SETFL, O_NONBLOCK);
        _clients.insert(std::make_pair(clientFd, Client(clientFd)));
        pollfd clientPollFd;
        clientPollFd.fd = clientFd;
        clientPollFd.events = POLLIN;
        clientPollFd.revents = 0;
        _pollFds.push_back(clientPollFd);

        std::cout << "New client Connected: FD " << clientFd << std::endl;
    }
}

std::vector<std::string> split(const std::string message)
{
	std::vector<std::string>	res;
	std::istringstream			iss(message);
	std::string					word;

	while (iss >> word)
		res.push_back(word);
	return (res);
}

bool	isNewNick(std::map<int, Client> &clients, std::string nickname)
{
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		Client client = it->second;
		if (client.getNickname().compare(nickname) == 0)
		{
			std::cout << "Nick " << nickname << " has already been registered. Choose another." << std::endl;
			return (false);
		}
	}
	return (true);
}

void    Server::processClientBuffer(Client &client)
{
	std::string &buf = client.getInputBuffer();
    size_t pos;

    while ((pos = buf.find("\r\n")) != std::string::npos)
    {
        std::string message = buf.substr(0, pos);
        buf.erase(0, pos + 1);
		std::vector<std::string> split_msg = split(message);
        std::cout << "Received command: " << message << std::endl;
		if (!split_msg.empty())
		{
			std::map<std::string, CommandHandler>::iterator it = _cmdMap.find(split_msg[0]);
			if (it != _cmdMap.end())
				(this->*(it->second))(client, message, split_msg);
			else
				sendMessage(client.getFd(), "421 " + client.getNickname() + " " + split_msg[0] + " :Unknown command");
			// if (split_msg[0].compare("PASS") == 0)
			// 	if (!authPass(client, split_msg[1], _password))
			// 		Server::removeClient(client.getFd());
			// if (split_msg[0].compare("NICK") == 0 && isNewNick(_clients, split_msg[1]))
			// 	setClientNick(split_msg[1], client);
			// if (split_msg[0].compare("USER") == 0)
			// 	setClientUsername(message, split_msg, client);
		}
		std::cout << client.getNickname() << std::endl;
		std::cout << client.getUsername() << std::endl;
		std::cout << client.getRealname() << std::endl;

		// TODO message if command has no parameters
    }
}

void    Server::removeClient(int clientFd)
{
    close(clientFd);
    _clients.erase(clientFd);
    for (size_t i = 0; i < _pollFds.size(); i++)
    {
        if (_pollFds[i].fd == clientFd)
        {
            _pollFds.erase(_pollFds.begin() + i);
            break;
        }
    }
}

void    Server::sendMessage(int clientFd, const std::string &message)
{
    std::string formatted = message + "\r\n";
    ssize_t bytes_send;

    bytes_send = send(clientFd, formatted.c_str(), formatted.size(), 0);
    if (bytes_send < 0)
        std::cerr << "Send failed to client fd: " << clientFd << std::endl;
}

void    Server::broadcastToChannel(const std::string &channelName, const std::string &message, int excludeFd)
{
    std::map<std::string, Channel>::iterator it = _channels.find(channelName);

    if (it == _channels.end())
        return;
    const std::set<int> &clients = it->second.getClients();
    for (std::set<int>::const_iterator index = clients.begin(); index != clients.end(); ++index)
    {
        if (*index == excludeFd)
            continue;
        sendMessage(*index, message);
    }
}

void	Server::handlePass(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens)
{
	(void)rawMsg;
	if (!authPass(client, tokens[1], _password))
		Server::removeClient(client.getFd());
}

void	Server::handleNick(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens)
{
	if (rawMsg.compare("NICK") == 0 && isNewNick(_clients, tokens[1]))
		setClientNick(tokens[1], client);

}

void	Server::handleUser(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens)
{
	if (tokens[0].compare("USER") == 0)
		setClientUsername(rawMsg, tokens, client);

}

void	Server::initCommandMap()
{
	_cmdMap["PASS"] = &Server::handlePass;
	_cmdMap["NICK"] = &Server::handleNick;
	_cmdMap["USER"] = &Server::handleUser;
}

void    Server::handleClientData(int clientFd)
{
    char    buffer[512];
    ssize_t bytes = recv(clientFd, buffer, sizeof(buffer), 0);

    if (bytes > 0)
        buffer[bytes] = '\0';
    else if (bytes == 0)
    {
        std::cout << "Client disconnected\n";
        removeClient(clientFd);
        return ;
    }
    std::map<int, Client>::iterator it = _clients.find(clientFd);
    if (it == _clients.end())
    {
        std::cerr << "Client not found!" << std::endl;
        return ;
    }
    Client &client = it->second;
    client.getInputBuffer().append(buffer, bytes);
    processClientBuffer(client);
}

void    Server::runPollLoop() {
    while (true)
    {
        int ready = poll(_pollFds.data(),_pollFds.size(), -1);
        if (ready < 0)
            throw std::runtime_error("Poll execution failed");
        for (size_t i = 0; i < _pollFds.size() && ready > 0; i++)
        {
            if (_pollFds[i].revents == 0)
                continue;
            if (_pollFds[i].revents & POLLIN)
            {
                if (_pollFds[i].fd == _serverSocketFd)
                {
                    // Accept new client
                    handleNewConnection();
                }
                else
                {
                    // Receive data from client
                    handleClientData(_pollFds[i].fd);
                }
            }
        }
    }
}

void    Server::start() {
    setupSocket();
    setupPolling();
    runPollLoop();
}
