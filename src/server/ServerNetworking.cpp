#include "../../includes/server/Server.hpp"
#include "../../includes/client/Client.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>
#include <vector>

void Server::setupSocketOpts()
{
	int opt = 1;
	if (setsockopt(_serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("Failed to set socket options");
}

void Server::setupServerAddress()
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = INADDR_ANY;
	std::memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
	if (bind(_serverSocketFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		throw std::runtime_error("Failed to bind socket port and address");
}

void Server::setupListen()
{
	if (listen(_serverSocketFd, SOMAXCONN) < 0)
		throw std::runtime_error("Failed to make the socket listen");
}

void Server::setupSocket()
{
	_serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocketFd < 0)
		throw std::runtime_error("Failed to create socket");
	setupSocketOpts();
	setupServerAddress();
	setupListen();
}

void Server::setupNonBlocking()
{
	int flags = fcntl(_serverSocketFd, F_GETFL, 0);
	if (flags < 0)
		throw std::runtime_error("Failed to get socket fd flags");
	if (fcntl(_serverSocketFd, F_SETFL, flags | O_NONBLOCK) < 0)
		throw std::runtime_error("Failed to set Non blocking on socket fd");
}

void Server::setupServerPoll()
{
	pollfd serverPoll;

	serverPoll.fd = _serverSocketFd;
	serverPoll.events = POLLIN;
	serverPoll.revents = 0;
	_pollFds.push_back(serverPoll);
}

void Server::setupPolling()
{
	setupNonBlocking();
	setupServerPoll();
}

void Server::handleNewConnection()
{
	while (true)
	{

		struct sockaddr_in clientAddr;
		socklen_t addrLen;

		addrLen = sizeof(clientAddr);
		int clientFd = accept(_serverSocketFd, (struct sockaddr *)&clientAddr, &addrLen);
		std::string clientHost = inet_ntoa(clientAddr.sin_addr);
		if (clientFd < 0)
		{
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				break;
			else
				throw std::runtime_error("Accept execution failed");
		}
		fcntl(clientFd, F_SETFL, O_NONBLOCK);
		_clients.insert(std::make_pair(clientFd, Client(clientFd, clientHost)));
		pollfd clientPollFd;
		clientPollFd.fd = clientFd;
		clientPollFd.events = POLLIN;
		clientPollFd.revents = 0;
		_pollFds.push_back(clientPollFd);
	}
}

void Server::handleClientData(int clientFd)
{
	char buffer[512];
	ssize_t bytes = recv(clientFd, buffer, sizeof(buffer), 0);

	if (bytes > 0)
		buffer[bytes] = '\0';
	else if (bytes == 0)
	{
		removeClient(clientFd);
		return;
	}
	std::map<int, Client>::iterator it = _clients.find(clientFd);
	if (it == _clients.end())
	{
		std::cerr << "Client not found!" << std::endl;
		return;
	}
	Client &client = it->second;
	client.getInputBuffer().append(buffer, bytes);
	processClientBuffer(client);
}

void Server::runPollLoop()
{
	while (true)
	{
		int ready = poll(_pollFds.data(), _pollFds.size(), -1);
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
					handleNewConnection();
				}
				else
				{
					handleClientData(_pollFds[i].fd);
				}
			}
		}
	}
}

void Server::start()
{
	setupSocket();
	setupPolling();
	runPollLoop();
}
