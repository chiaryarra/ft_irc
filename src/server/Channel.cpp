#include "../../includes/server/Channel.hpp"
#include <algorithm>
#include <sstream>

Channel::Channel() : _name(), _clients(), _operators(), _userLimit(0), _topicProtected(false) {}

Channel::Channel(const std::string &name) : _name(name), _clients(), _operators(), _userLimit(0), _topicProtected(false) {}

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
		_topicProtected = other._topicProtected;
	}
	return *this;
}

Channel::~Channel() { }

void Channel::addClient(int clientFd) { _clients.insert(clientFd); }

void Channel::removeClient(int clientFd)
{
	_clients.erase(clientFd);
	_operators.erase(clientFd);
}

void Channel::removeOperator(int clientFd) { _operators.erase(clientFd); }

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

const std::string &Channel::getTopic() const { return _topic; }

const std::string &Channel::getModes() const { return _modes; }

const std::string &Channel::getKey() const { return _key; }

const unsigned int &Channel::getUserLimit() const { return _userLimit; }

const bool &Channel::isInviteOnly() const { return _inviteOnly; }

const bool &Channel::isTopicProtected() const { return _topicProtected; }

void Channel::setTopic(std::string topic) { _topic = topic; }

void Channel::setModes(std::string &modes) { _modes = modes; }

bool Channel::addMode(char mode)
{
	if (_modes.find(mode) == std::string::npos)
	{
		switch (mode)
		{
		case 'l':
			_modes.push_back(mode);
			break;
		case 'k':
			if (_modes.size() >= 1 && _modes.at(_modes.size() - 1) == 'l')
				_modes.insert(_modes.size() - 1, 1, mode);
			else
				_modes.push_back(mode);
			break;
		default:
			_modes.insert(0, 1, mode);
		}
		return true;
	}
	return false;
}

const std::string Channel::getUserLimitStr() const
{
	std::ostringstream oss;

	oss << _userLimit;
	return oss.str();
}

void Channel::setKey(std::string key) { _key = key; }

void Channel::setUserLimit(unsigned int limit) { _userLimit = limit; }

void Channel::setInviteOnly(bool mode) { _inviteOnly = mode; }

void Channel::setTopicProtected(bool mode) { _topicProtected = mode; }

bool Channel::removeMode(char mode)
{
	if (_modes.find_first_of(mode) != std::string::npos)
	{
		_modes.erase(std::remove(_modes.begin(), _modes.end(), mode), _modes.end());
		return true;
	}
	return false;
}

void Channel::removeInvite(int clientFd) { _inviteList.erase(clientFd); }

bool Channel::isInvited(int clientFd) const { return (_inviteList.find(clientFd) != _inviteList.end()); }

bool Channel::hasMode(char mode) const { return (_modes.find(mode) != std::string::npos); }

void Channel::addInvite(int clientFd) { _inviteList.insert(clientFd); }
