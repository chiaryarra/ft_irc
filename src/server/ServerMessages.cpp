#include "../../includes/server/Server.hpp"
#include "../../includes/client/Client.hpp"
#include "../../includes/utils/Utils.hpp"
#include <iostream>
#include <set>
#include <string>
#include <sys/socket.h>

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

void Server::sendError(Client &client, const std::string &command, const std::string &errorCode, const std::string &extra)
{
	std::map<std::string, std::string>::iterator it = _errorDescriptions.find(errorCode);
	if (it != _errorDescriptions.end())
		sendMessage(client.getFd(), ":" + _serverName + " " + (errorCode == "900" || errorCode == "901" || errorCode == "902" || errorCode == "903" ? ERR_NEEDMOREPARAMS : errorCode) + " " + command + " " + extra + (extra.empty() ? "" : " ") + it->second);
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

std::string Server::showChannelModes(Channel &channel)
{
	std::string message;

	message += channel.getModes().empty() ? "" : "+" + channel.getModes();
	message += channel.getKey().empty() ? "" : " " + channel.getKey();
	message += channel.getUserLimit() == 0 ? "" : " " + channel.getUserLimitStr();

	return message;
}

void Server::sendJoinMessage(Client &client, Channel &channel)
{
	sendMessage(client.getFd(), ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHost() + " JOIN " + ":" + channel.getName());
	sendMessage(client.getFd(), ":" + _serverName + " " + RPL_NOTOPIC + " " + client.getNickname() + " " + channel.getName() + " " + (channel.getTopic().empty() ? ":No topic is set" : channel.getTopic()));
	sendMessage(client.getFd(), ":" + _serverName + " " + RPL_NAMREPLY + " " + client.getNickname() + " = " + channel.getName() + " " + ":" + showClientsInChannel(channel));
	sendMessage(client.getFd(), ":" + _serverName + " " + RPL_ENDOFNAMES + " " + client.getNickname() + " " + channel.getName() + " :End of /NAMES list.");
}
