#include "../../includes/server/Server.hpp"
#include "../../includes/client/Client.hpp"
#include "../../includes/utils/Utils.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>
#include <vector>

Server::Server(int port, const std::string &password) : _serverName("ircat"), _version("0.5"), _creationDate(std::string(__DATE__) + " " + __TIME__)
{
	_port = port;
	_password = password;
	_serverSocketFd = -1;
	_running = false;
	initCommandMap();
	initErrorDescriptions();
}

Server::~Server()
{
	// Will need to have the close sockets fds, free memory and delete clients
	// later
	if (_serverSocketFd >= 0)
		close(_serverSocketFd);
}

void Server::setupSocketOpts()
{
	// Setup the options that our server socket will have
	int opt = 1;
	if (setsockopt(_serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("Failed to set socket options");
}

void Server::setupServerAddress()
{
	// Setup the address and port that our socket will be listening
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

		std::cout << "New client Connected: FD " << clientFd << std::endl;
	}
}

std::vector<std::string> split(const std::string message)
{
	std::vector<std::string> res;
	std::istringstream iss(message);
	std::string word;

	while (iss >> word)
		res.push_back(word);
	return (res);
}

void Server::processClientBuffer(Client &client)
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
				sendMessage(client.getFd(), ERR_UNKNOWNCOMMAND + " " + (client.getNickname().empty() ? "*" : client.getNickname()) + " " + split_msg[0] + " :Unknown command");
		}
		if (!client.getIsRegistered() && client.getIsAuthenticated() && !client.getNickname().empty() && !client.getUsername().empty())
		{
			sendWelcomeMessage(client);
			client.setIsRegistered(true);
		}
	}
}

void Server::removeClient(int clientFd)
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

void Server::sendMessage(int clientFd, const std::string &message)
{
	std::string formatted = message + "\r\n";
	ssize_t bytes_send;

	bytes_send = send(clientFd, formatted.c_str(), formatted.size(), 0);
	if (bytes_send < 0)
		std::cerr << "Send failed to client fd: " << clientFd << std::endl;
}

void Server::sendWelcomeMessage(Client &client)
{
	sendMessage(client.getFd(), ":" + _serverName + " " + RPL_WELCOME + " " + client.getNickname() + " :Welcome to our IRC network " + client.getNickname() + "!" + client.getUsername() + "@" + client.getHost());
	sendMessage(client.getFd(), ":" + _serverName + " " + RPL_YOURHOST + " " + client.getNickname() + " :Your host is " + _serverName + ", running version " + _version);
	sendMessage(client.getFd(), ":" + _serverName + " " + RPL_CREATED + " " + client.getNickname() + " :This server was created at " + _creationDate);
	sendMessage(client.getFd(), ":" + _serverName + " " + RPL_MYINFO + " " + client.getNickname() + " " + _serverName + " " + _version + " o o");
}

void Server::broadcastToChannel(const std::string &channelName, const std::string &message, int excludeFd)
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

void Server::sendError(Client &client, const std::string &command, const std::string &errorCode)
{
	std::map<std::string, std::string>::iterator it = _errorDescriptions.find(errorCode);
	if (it != _errorDescriptions.end())
		sendMessage(client.getFd(), errorCode + " " + command + " " + it->second);
}

void Server::handlePass(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens)
{
	std::string res;

	(void)rawMsg;
	if (tokens.size() < 2)
	{
		sendMessage(client.getFd(), ERR_NEEDMOREPARAMS + " PASS " + MSG_NEEDMOREPARAMS);
		return;
	}
	res = authPass(client, tokens[1], _password);
	if (!res.empty())
	{
		sendError(client, "PASS", res);
		if (res.compare(ERR_PASSWDMISMATCH) == 0)
			removeClient(client.getFd());
	}
}

void Server::handleNick(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens)
{
	std::string res;

	(void)rawMsg;
	if (tokens.size() < 2)
	{
		sendMessage(client.getFd(), ERR_NEEDMOREPARAMS + " NICK " + MSG_NEEDMOREPARAMS);
		return;
	}
	res = setClientNick(tokens[1], client, _clients);
	if (res.compare(ERR_ERRONEUSNICKNAME) == 0)
		sendMessage(client.getFd(), ERR_ERRONEUSNICKNAME + " NICK " + ":Erroneous Nickname");
	if (res.compare(ERR_NICKNAMEINUSE) == 0)
		sendMessage(client.getFd(), ERR_NICKNAMEINUSE + " NICK " + ":Nickname is already in use");
}

void Server::handleUser(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens)
{
	std::string res;

	res = setClientUsername(rawMsg, tokens, client);
	if (tokens.size() < 5)
	{
		sendMessage(client.getFd(), ERR_NEEDMOREPARAMS + " USER " + MSG_NEEDMOREPARAMS);
		return;
	}
	if (res.compare(ERR_INVALIDUSERNAME) == 0)
		sendMessage(client.getFd(), ERR_NEEDMOREPARAMS + " USER " + ":invalid username");
	if (res.compare(ERR_INVALIDMODE) == 0)
		sendMessage(client.getFd(), ERR_NEEDMOREPARAMS + " USER " + ":invalid mode (not 0)");
	if (res.compare(ERR_INVALIDUNUSED) == 0)
		sendMessage(client.getFd(), ERR_NEEDMOREPARAMS + " USER " + ":invalid unused (not *)");
	if (res.compare(ERR_INVALIDREALNAME) == 0)
		sendMessage(client.getFd(), ERR_NEEDMOREPARAMS + " USER " + ":invalid realname");
}

std::string Server::showClientsInChannel(Channel &channel)
{
	std::string names;

	for (std::set<int>::iterator it = channel.getClients().begin(); it != channel.getClients().end(); ++it)
	{

		std::set<int>::iterator next_it = it;
		++next_it;
		std::map<int, Client>::iterator client_it = _clients.find(*it);

		std::set<int>::iterator op_it = channel.getOperators().find(*it);
		if (op_it != channel.getOperators().end())
			names += "@";

		std::cout << client_it->second.getNickname();
		names += client_it->second.getNickname();
		if (next_it != channel.getClients().end())
			names += " ";
	}
	return names;
}

void Server::sendJoinMessage(Client &client, Channel &channel)
{
	sendMessage(client.getFd(), ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHost() + " JOIN " + ":" + channel.getName());
	sendMessage(client.getFd(), ":" + _serverName + " " + RPL_NOTOPIC + " " + client.getNickname() + " " + channel.getName() + " " + (channel.getTopic().empty() ? ":No topic is set" : channel.getTopic()));
	sendMessage(client.getFd(), ":" + _serverName + " " + RPL_NAMREPLY + " " + client.getNickname() + " = " + channel.getName() + " " + ":" + showClientsInChannel(channel));
	sendMessage(client.getFd(), ":" + _serverName + " " + RPL_ENDOFNAMES + " " + client.getNickname() + " " + channel.getName() + " :End of /NAMES list.");
}

void Server::handleJoin(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens)
{
	std::string res;
	bool isNew;
	bool isKeyPass;

	(void)rawMsg;
	isNew = false;
	isKeyPass = false;
	if (tokens.size() < 2)
	{
		sendMessage(client.getFd(), ERR_NEEDMOREPARAMS + " JOIN " + MSG_NEEDMOREPARAMS);
		return;
	}
	bool isNewChannel = _channels.find(tokens[1]) != _channels.end() ? true : false;
	res = joinChannel(client, tokens[1], isNewChannel);

	if (res.compare(ERR_NOTREGISTERED) == 0)
	{
		sendMessage(client.getFd(), ERR_NOTREGISTERED + " JOIN " + ":Not registered");
		return;
	}
	if (res.compare(ERR_NOSUCHCHANNEL) == 0)
	{
		sendMessage(client.getFd(), ERR_NOSUCHCHANNEL + " JOIN " + ":No such channel");
		return;
	}

	std::map<std::string, Channel>::iterator it = _channels.find(tokens[1]);
	if (it == _channels.end())
	{
		it = _channels.insert(std::pair<std::string, Channel>(tokens[1], Channel(tokens[1]))).first;
		isNew = true;
	}

	Channel &channel = it->second;
	if (tokens.size() >= 3)
		isKeyPass = channel.getKey().compare(tokens[2]) == 0;

	res = checkChannelMode(channel, client.getFd(), isKeyPass);
	if (res.compare(ERR_CHANNELISFULL) == 0)
	{
		sendMessage(client.getFd(), res + " JOIN " + ":Channel is full");
		return;
	}
	if (res.compare(ERR_INVITEONLYCHAN) == 0)
	{
		sendMessage(client.getFd(), ERR_INVITEONLYCHAN + " JOIN " + ":Client not invited");
		return;
	}
	if (res.compare(ERR_BADCHANNELKEY) == 0)
	{
		sendMessage(client.getFd(), ERR_BADCHANNELKEY + " JOIN " + ":Wrong key");
		return;
	}

	channel.addClient(client.getFd());
	if (isNew)
		channel.addOperator(client.getFd());
	sendJoinMessage(client, channel);
}

void Server::handleCap(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens)
{
	(void)rawMsg;
	(void)tokens;
	std::cout << "Ignoring CAP command from FD: " << client.getFd() << std::endl;
	return;
}

void Server::handlePing(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens)
{
	if (tokens.size() < 2)
	{
		sendMessage(client.getFd(), ERR_NEEDMOREPARAMS + " PING " + MSG_NEEDMOREPARAMS);
		return;
	}
	size_t pos = rawMsg.find_first_of(":");
	if (pos != std::string::npos)
		sendMessage(client.getFd(), "PONG " + rawMsg.substr(pos));
	else
	{
		std::string msg;
		std::vector<std::string>::const_iterator last = tokens.end();
		last++;
		for (std::vector<std::string>::const_iterator it = tokens.begin() + 1; it != tokens.end(); ++it)
		{
			msg.append(*it);
			if (it != last)
				msg.append(" ");
		}
		sendMessage(client.getFd(), "PONG " + msg);
	}
}

void Server::initCommandMap()
{
	_cmdMap["PASS"] = &Server::handlePass;
	_cmdMap["NICK"] = &Server::handleNick;
	_cmdMap["USER"] = &Server::handleUser;
	_cmdMap["JOIN"] = &Server::handleJoin;
	_cmdMap["CAP"] = &Server::handleCap;
	_cmdMap["PING"] = &Server::handlePing;
}

void Server::initErrorDescriptions()
{
	_errorDescriptions[ERR_ALREADYREGISTRED] = ":You may not reregister";
	_errorDescriptions[ERR_PASSWDMISMATCH] = ":Password incorrect";
	_errorDescriptions[ERR_ERRONEUSNICKNAME] = ":Erroneous Nickname";
	_errorDescriptions[ERR_NICKNAMEINUSE] = ":Nickname is already in use";
	_errorDescriptions[ERR_NOTREGISTERED] = ":Not registered";
	_errorDescriptions[ERR_NOSUCHCHANNEL] = ":No such channel";
	_errorDescriptions[ERR_CHANNELISFULL] = ":Channel is full";
	_errorDescriptions[ERR_INVITEONLYCHAN] = ":Client not invited";
	_errorDescriptions[ERR_BADCHANNELKEY] = ":Wrong key";
	_errorDescriptions[ERR_INVALIDUSERNAME] = ":invalid username";
	_errorDescriptions[ERR_INVALIDMODE] = ":invalid mode (not 0)";
	_errorDescriptions[ERR_INVALIDUNUSED] = ":invalid unused (not *)";
	_errorDescriptions[ERR_INVALIDREALNAME] = ":invalid realname";
}

void Server::handleClientData(int clientFd)
{
	char buffer[512];
	ssize_t bytes = recv(clientFd, buffer, sizeof(buffer), 0);

	if (bytes > 0)
		buffer[bytes] = '\0';
	else if (bytes == 0)
	{
		std::cout << "Client disconnected\n";
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

void Server::start()
{
	setupSocket();
	setupPolling();
	runPollLoop();
}
