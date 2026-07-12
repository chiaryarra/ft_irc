#include "../../includes/server/Channel.hpp"
#include <algorithm>
#include <iostream>

Channel::Channel() : _name(), _clients(), _operators() {}

Channel::Channel(const std::string &name) : _name(name), _clients(), _operators() {}

Channel &Channel::operator=(const Channel &other)
{
	if (this != &other)
	{
		_name = other._name;
		_clients = other._clients;
		_operators = other._operators;
		_topic = other._topic;
		_modes = other._modes;
		_key = other._key;
		_inviteList = other._inviteList;
		_userLimit = other._userLimit;
	}
	return *this;
}

void Channel::addClient(int clientFd) { _clients.insert(clientFd); }

void Channel::removeClient(int clientFd)
{
	_clients.erase(clientFd);
	_operators.erase(clientFd);
}

void Channel::addOperator(int clientFd)
{
	if (_clients.count(clientFd))
		_operators.insert(clientFd);
}

bool Channel::isMember(int clientFd) const { return (_clients.count(clientFd)); }

bool Channel::isOperator(int clientFd) const { return (_operators.count(clientFd)); }

const std::string &Channel::getName() const { return _name; }

const std::set<int> &Channel::getClients() const { return _clients; }

const std::set<int> &Channel::getOperators() const { return _operators; };

// std::set<int> &Channel::getOperators() { return _operators; };

const std::string &Channel::getTopic() const { return _topic; }

const std::string &Channel::getModes() const { return _modes; }

const std::string &Channel::getKey() const { return _key; }

const unsigned int &Channel::getUserLimit() const { return _userLimit; }

void Channel::setTopic(std::string topic) { _topic = topic; }

void Channel::setModes(std::string &modes) { _modes = modes; }

void Channel::addMode(char mode)
{
	if (_modes.find(mode) == std::string::npos)
		_modes += mode;
}

void Channel::setKey(std::string key) { _key = key; }

void Channel::setUserLimit(unsigned int limit) { _userLimit = limit; }

void Channel::removeMode(char mode) { _modes.erase(std::remove(_modes.begin(), _modes.end(), mode), _modes.end()); }

void Channel::removeInvite(int clientFd) { _inviteList.erase(clientFd); }

bool Channel::isInvited(int clientFd) const { return (_inviteList.find(clientFd) != _inviteList.end()); }

bool Channel::hasMode(char mode) const { return (_modes.find(mode) != std::string::npos); }

void Channel::addInvite(int clientFd) { _inviteList.insert(clientFd); }
