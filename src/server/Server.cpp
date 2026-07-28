#include "../../includes/server/Server.hpp"
#include "../../includes/client/Client.hpp"
#include "../../includes/utils/Utils.hpp"
#include <iostream>
#include <set>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>

Server::Server(int port, const std::string &password)
    : _serverName("ircat"), _version("0.5"),
      _creationDate(std::string(__DATE__) + " " + __TIME__) {
  _port = port;
  _password = password;
  _serverSocketFd = -1;
  _running = false;
  initCommandMap();
  initErrorDescriptions();
}

Server::~Server() {
  // Will need to have the close sockets fds, free memory and delete clients
  // later
  if (_serverSocketFd >= 0)
    close(_serverSocketFd);
}

void Server::processClientBuffer(Client &client)
{
	std::string &buf = client.getInputBuffer();
	size_t pos;

  while ((pos = buf.find("\r\n")) != std::string::npos) {
    std::string message = buf.substr(0, pos);
    buf.erase(0, pos + 2);
    std::vector<std::string> split_msg = split(message);
    std::cout << "Received command: " << message << std::endl;
    if (!split_msg.empty()) {
      std::map<std::string, CommandHandler>::iterator it =
          _cmdMap.find(split_msg[0]);
      if (it != _cmdMap.end())
        (this->*(it->second))(client, message, split_msg);
      else
        sendMessage(
            client.getFd(),
            ERR_UNKNOWNCOMMAND + " " +
                (client.getNickname().empty() ? "*" : client.getNickname()) +
                " " + split_msg[0] + " :Unknown command");
    }
    if (!client.getIsRegistered() && client.getIsAuthenticated() &&
        !client.getNickname().empty() && !client.getUsername().empty()) {
      sendWelcomeMessage(client);
      client.setIsRegistered(true);
    }
  }
}

void Server::removeClient(int clientFd)
{
	std::map<std::string, Channel>::iterator it = _channels.begin();
	while (it != _channels.end())
	{
		if (it->second.isMember(clientFd))
		{
			it->second.removeClient(clientFd);
			if (it->second.getClients().empty())
				_channels.erase(it++);
			else
				++it;
		}
		else
			++it;
	}
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

void Server::handlePass(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens)
{
	std::string res;

  (void)rawMsg;
  if (tokens.size() < 2) {
    sendError(client, "PASS", ERR_NEEDMOREPARAMS);
    return;
  }
  res = authPass(client, tokens[1], _password);
  if (!res.empty() && res.compare(RPL_SUCCESS) != 0) {
    sendError(client, "PASS", res);
    if (res.compare(ERR_PASSWDMISMATCH) == 0)
      removeClient(client.getFd());
  }
}

void Server::handleNick(Client &client, const std::string &rawMsg,
                        const std::vector<std::string> &tokens) {
  std::string res;

  (void)rawMsg;
  if (tokens.size() < 2) {
    sendError(client, "NICK", ERR_NEEDMOREPARAMS);
    return;
  }
  res = setClientNick(tokens[1], client, _clients);
  if (!res.empty() && res.compare(RPL_SUCCESS) != 0)
    sendError(client, "NICK", res);
}

void Server::handleUser(Client &client, const std::string &rawMsg,
                        const std::vector<std::string> &tokens) {
  std::string res;

  (void)rawMsg;
  if (tokens.size() < 5) {
    sendError(client, "USER", ERR_NEEDMOREPARAMS);
    return;
  }
  res = setClientUsername(tokens, client);
  if (!res.empty() && res.compare(RPL_SUCCESS) != 0)
    sendError(client, "USER", res);
}

void Server::handleJoin(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens)
{
	std::string res;
	bool isNew;
	bool isKeyPass;

  (void)rawMsg;
  isNew = false;
  isKeyPass = false;
  if (tokens.size() < 2) {
    sendError(client, "JOIN", ERR_NEEDMOREPARAMS);
    return;
  }
  bool isNewChannel =
      _channels.find(tokens[1]) != _channels.end() ? true : false;
  res = joinChannel(client, tokens[1], isNewChannel);
  if (!res.empty() && res.compare(RPL_SUCCESS) != 0) {
    sendError(client, "JOIN", res);
    return;
  }

  std::map<std::string, Channel>::iterator it = _channels.find(tokens[1]);
  if (it == _channels.end()) {
    it = _channels
             .insert(
                 std::pair<std::string, Channel>(tokens[1], Channel(tokens[1])))
             .first;
    isNew = true;
  }

  Channel &channel = it->second;
  if (tokens.size() >= 3)
    isKeyPass = channel.getKey().compare(tokens[2]) == 0;

  res = checkChannelMode(channel, client.getFd(), isKeyPass);
  if (!res.empty() && res.compare(RPL_SUCCESS) != 0) {
    sendError(client, "JOIN", res);
    return;
  }

  channel.addClient(client.getFd());
  if (isNew)
    channel.addOperator(client.getFd());
  sendJoinMessage(client, channel);
}

void Server::handleCap(Client &client, const std::string &rawMsg,
                       const std::vector<std::string> &tokens) {
  (void)rawMsg;
  (void)tokens;
  std::cout << "Ignoring CAP command from FD: " << client.getFd() << std::endl;
  return;
}

void Server::handlePing(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens)
{
	std::string msg;
	std::vector<std::string>::const_iterator last = tokens.end() - 1;

	(void)rawMsg;
	if (tokens.size() < 2)
	{
		sendMessage(client.getFd(), ERR_NEEDMOREPARAMS + " PING " + MSG_NEEDMOREPARAMS);
		return;
	}
	last = tokens.end() - 1;
	for (std::vector<std::string>::const_iterator it = tokens.begin() + 1; it != tokens.end(); ++it)
	{
		msg.append(*it);
		if (it != last)
			msg.append(" ");
	}
	sendMessage(client.getFd(), "PONG " + msg);
}

void Server::handleMode(Client &client, const std::string &rawMsg,
                        const std::vector<std::string> &tokens) {
  std::string res;
  std::string modes;
  std::vector<std::string> params;
  std::map<std::string, Channel>::iterator chanIt;
  std::string modeChange;

  (void)rawMsg;
  if (tokens.size() < 2) {
    sendError(client, "MODE", ERR_NEEDMOREPARAMS);
    return;
  }
  chanIt = _channels.find(tokens[1]);
  if (chanIt == _channels.end()) {
    sendError(client, "MODE", ERR_NOSUCHCHANNEL);
    return;
  }
  if (tokens.size() >= 3)
    modes = tokens[2];
  if (tokens.size() >= 4)
    params = std::vector<std::string>(tokens.begin() + 3, tokens.end());

  res =
      manageChannelMode(chanIt->second, modes, params, _clients,
                        chanIt->second.isMember(client.getFd()),
                        chanIt->second.isOperator(client.getFd()), modeChange);

  if (res.compare(RPL_CHANNELMODEIS) == 0) {
    sendMessage(client.getFd(), ":" + _serverName + " " + RPL_CHANNELMODEIS +
                                    " " + client.getNickname() + " " +
                                    chanIt->second.getName() + " " +
                                    showChannelModes(chanIt->second));
    return;
  } else if (res.find(ERR_UNKNOWNMODE) == 0 &&
             res.size() > ERR_UNKNOWNMODE.size()) {
    sendError(client, "MODE", ERR_UNKNOWNMODE,
              res.substr(ERR_UNKNOWNMODE.size() + 1));
    return;
  } else if (res.compare(RPL_SUCCESS) != 0) {
    sendError(client, "MODE", res);
    return;
  }
  if (!modeChange.empty())
    broadcastToChannel(chanIt->second.getName(),
                       ":" + client.getNickname() + "!" + client.getUsername() +
                           "@" + client.getHost() + " MODE " +
                           chanIt->second.getName() + " " + modeChange,
                       client.getFd());
}

void Server::handleQuit(Client &client, const std::string &rawMsg,
                        const std::vector<std::string> &tokens) {
  std::vector<Channel> currentChannels;
  std::set<int> mutualClients;

  (void)rawMsg;

	for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		if (it->second.isMember(client.getFd()))
			currentChannels.push_back(it->second);
	for (std::vector<Channel>::iterator it = currentChannels.begin(); it != currentChannels.end(); ++it)
		mutualClients.insert(it->getClients().begin(), it->getClients().end());
	for (std::set<int>::iterator it = mutualClients.begin(); it != mutualClients.end(); ++it)
		sendMessage(*it, ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHost() + " QUIT " + ":" + (tokens.size() > 1 ? tokens[1] : "Client quit"));
	removeClient(client.getFd());
}

void Server::handlePart(Client &client, const std::string &rawMsg,
                        const std::vector<std::string> &tokens) {
  std::map<std::string, Channel>::iterator chanIt;
  std::string res;

  (void)rawMsg;
  if (tokens.size() < 2) {
    sendError(client, "PART", ERR_NEEDMOREPARAMS);
    return;
  }
  chanIt = _channels.find(tokens[1]);
  if (chanIt == _channels.end()) {
    sendError(client, "PART", ERR_NOSUCHCHANNEL);
    return;
  }
  res = managePartCommand(client, chanIt->second);
  if (res.compare(ERR_NOTONCHANNEL) == 0)
    sendError(client, "PART", res);
  else {
    broadcastToChannel(chanIt->second.getName(),
                       ":" + client.getNickname() + "!" + client.getUsername() +
                           "@" + client.getHost() + " PART " +
                           chanIt->second.getName() +
                           ((tokens.size() > 2) ? (" :" + tokens[2]) : ""),
                       0);
    if (chanIt->second.isOperator(client.getFd()))
      chanIt->second.removeOperator(client.getFd());
    chanIt->second.removeClient(client.getFd());
    if (chanIt->second.getClients().empty())
      _channels.erase(chanIt->first);
  }
}

void Server::handleInvite(Client &client, const std::string &rawMsg,
                          const std::vector<std::string> &tokens) {
  std::map<std::string, Channel>::iterator chanIt;
  std::map<int, Client>::iterator targetIt;
  std::string res;

  (void)rawMsg;
  if (tokens.size() < 3) {
    sendError(client, "INVITE", ERR_NEEDMOREPARAMS);
    return;
  }
  chanIt = _channels.find(tokens[2]);
  targetIt = findClientByNick(_clients, tokens[1]);
  if (chanIt == _channels.end()) {
    sendError(client, "INVITE", ERR_NOSUCHCHANNEL);
    return;
  }
  if (targetIt == _clients.end()) {
    sendError(client, "INVITE", ERR_NOSUCHNICK);
    return;
  }
  res = inviteUser(client, targetIt->second, chanIt->second);
  if (res.compare(RPL_INVITING) != 0) {
    sendError(client, "INVITE", res);
    return;
  }
  sendMessage(client.getFd(), ":" + _serverName + " " + res + " " +
                                  client.getNickname() + " " +
                                  targetIt->second.getNickname() + " " +
                                  chanIt->second.getName());
  sendMessage(targetIt->second.getFd(), ":" + client.getNickname() + "!" +
                                            client.getUsername() + "@" +
                                            client.getHost() + " " + "INVITE " +
                                            targetIt->second.getNickname() +
                                            " :" + chanIt->second.getName());
}

void Server::handleTopic(Client &client, const std::string &rawMsg,
                         const std::vector<std::string> &tokens) {
  std::map<std::string, Channel>::iterator chanIt;
  std::string res;

  (void)rawMsg;
  if (tokens.size() < 2) {
    sendError(client, "INVITE", ERR_NEEDMOREPARAMS);
    return;
  }
  chanIt = _channels.find(tokens[1]);
  if (chanIt == _channels.end()) {
    sendError(client, "TOPIC", ERR_NOSUCHCHANNEL);
    return;
  }
  res = manageChannelTopic(client, chanIt->second, tokens);
  if (res.compare(ERR_NOTONCHANNEL) == 0 ||
      res.compare(ERR_CHANOPRIVSNEEDED) == 0)
    sendError(client, "TOPIC", res);
  else if (res.compare(RPL_TOPIC) == 0)
    sendMessage(client.getFd(), ":" + _serverName + " " + res + " " +
                                    client.getNickname() + " " +
                                    chanIt->second.getName() + " :" +
                                    chanIt->second.getTopic());
  else if (res.compare(RPL_NOTOPIC) == 0)
    sendMessage(client.getFd(),
                ":" + _serverName + " " + res + " " + client.getNickname() +
                    " " + chanIt->second.getName() + " :No topic is set");
  else
    broadcastToChannel(chanIt->second.getName(),
                       ":" + client.getNickname() + "!" + client.getUsername() +
                           "@" + client.getHost() + " TOPIC " +
                           chanIt->second.getName() + " :" +
                           chanIt->second.getTopic(),
                       client.getFd());
}

void Server::handlePrivmsg(Client &client, const std::string &rawMsg,
                           const std::vector<std::string> &tokens) {
  std::map<std::string, Channel>::iterator chanIt;
  std::map<int, Client>::iterator targetIt;
  std::string res;

  (void)rawMsg;
  if (tokens.size() < 3) {
    sendError(client, "PRIVMSG", ERR_NEEDMOREPARAMS);
    return;
  }
  if (tokens[1].at(0) == '#') {
    chanIt = _channels.find(tokens[1]);
    if (chanIt == _channels.end()) {
      sendError(client, "PRIVMSG", ERR_NOSUCHCHANNEL);
      return;
    }
    res = managePrivmsgToChannel(client, chanIt->second, tokens[2]);
    if (res.compare(RPL_SUCCESS) != 0) {
      sendError(client, "PRIVMSG", res);
      return;
    }
    broadcastToChannel(chanIt->second.getName(),
                       ":" + client.getNickname() + "!" + client.getUsername() +
                           "@" + client.getHost() + " PRIVMSG " +
                           chanIt->second.getName() + " :" + tokens[2],
                       client.getFd());
  } else {
    targetIt = findClientByNick(_clients, tokens[1]);
    if (targetIt == _clients.end()) {
      sendError(client, "PRIVMSG", ERR_NOSUCHNICK);
      return;
    }
    res = managePrivmsgToClient(tokens[2]);
    if (res.compare(RPL_SUCCESS) != 0) {
      sendError(client, "PRIVMSG", res);
      return;
    }
    sendMessage(targetIt->second.getFd(),
                ":" + client.getNickname() + "!" + client.getUsername() + "@" +
                    client.getHost() + " PRIVMSG " +
                    targetIt->second.getNickname() + " :" + tokens[2]);
  }
}

void Server::handleKick(Client &client, const std::string &rawMsg,
                        const std::vector<std::string> &tokens) {
  std::map<std::string, Channel>::iterator chanIt;
  std::map<int, Client>::iterator targetIt;
  std::string res;

  (void)rawMsg;
  if (tokens.size() < 3) {
    sendError(client, "KICK", ERR_NEEDMOREPARAMS);
    return;
  }
  chanIt = _channels.find(tokens[1]);
  targetIt = findClientByNick(_clients, tokens[2]);
  if (chanIt == _channels.end()) {
    sendError(client, "KICK", ERR_NOSUCHCHANNEL);
    return;
  }
  if (targetIt == _clients.end()) {
    sendError(client, "KICK", ERR_NOSUCHNICK);
    return;
  }
  res = manangeKickCommand(client, chanIt->second, targetIt->second);
  if (res.compare(RPL_SUCCESS) != 0) {
    sendError(client, "KICK", res);
    return;
  }
  broadcastToChannel(chanIt->second.getName(),
                     ":" + client.getNickname() + "!" + client.getUsername() +
                         "@" + client.getHost() + " KICK " +
                         chanIt->second.getName() + " " +
                         targetIt->second.getNickname() +
                         ((tokens.size() > 3) ? (" :" + tokens[3]) : ""),
                     0);
  if (chanIt->second.isOperator(targetIt->second.getFd()))
    chanIt->second.removeOperator(targetIt->second.getFd());
  chanIt->second.removeClient(targetIt->second.getFd());
}

void Server::initCommandMap() {
  _cmdMap["PASS"] = &Server::handlePass;
  _cmdMap["NICK"] = &Server::handleNick;
  _cmdMap["USER"] = &Server::handleUser;
  _cmdMap["JOIN"] = &Server::handleJoin;
  _cmdMap["CAP"] = &Server::handleCap;
  _cmdMap["PING"] = &Server::handlePing;
  _cmdMap["MODE"] = &Server::handleMode;
  _cmdMap["QUIT"] = &Server::handleQuit;
  _cmdMap["INVITE"] = &Server::handleInvite;
  _cmdMap["KICK"] = &Server::handleKick;
  _cmdMap["TOPIC"] = &Server::handleTopic;
  _cmdMap["PRIVMSG"] = &Server::handlePrivmsg;
  _cmdMap["PART"] = &Server::handlePart;
}

void Server::initErrorDescriptions() {
  _errorDescriptions[ERR_ALREADYREGISTRED] = ":You may not reregister";
  _errorDescriptions[ERR_PASSWDMISMATCH] = ":Password incorrect";
  _errorDescriptions[ERR_ERRONEUSNICKNAME] = ":Erroneous Nickname";
  _errorDescriptions[ERR_NICKNAMEINUSE] = ":Nickname is already in use";
  _errorDescriptions[ERR_NOTREGISTERED] = ":Not registered";
  _errorDescriptions[ERR_NOSUCHCHANNEL] = ":No such channel";
  _errorDescriptions[ERR_NOTONCHANNEL] = ":You're not on that channel";
  _errorDescriptions[ERR_NOSUCHNICK] = ":No such nick/channel";
  _errorDescriptions[ERR_CHANNELISFULL] = ":Channel is full";
  _errorDescriptions[ERR_CHANOPRIVSNEEDED] = ":You're not channel operator";
  _errorDescriptions[ERR_USERNOTINCHANNEL] = ":They aren't on that channel";
  _errorDescriptions[ERR_INVITEONLYCHAN] = ":Client not invited";
  _errorDescriptions[ERR_BADCHANNELKEY] = ":Wrong key";
  _errorDescriptions[ERR_INVALIDUSERNAME] = ":invalid username";
  _errorDescriptions[ERR_INVALIDMODE] = ":invalid mode (not 0)";
  _errorDescriptions[ERR_NORECIPIENT] = "No recipient";
  _errorDescriptions[ERR_NOTEXTTOSEND] = "No text to send";
  _errorDescriptions[ERR_CANNOTSENDTOCHAN] = "Cannot send to channel";
  _errorDescriptions[ERR_INVALIDUNUSED] = ":invalid unused (not *)";
  _errorDescriptions[ERR_INVALIDREALNAME] = ":invalid realname";
  _errorDescriptions[ERR_UNKNOWNMODE] = ":is unknown mode char to me";
  _errorDescriptions[ERR_NEEDMOREPARAMS] = ":not enough parameters";
  _errorDescriptions[ERR_UNKNOWNERROR] = ":unknown error";
}