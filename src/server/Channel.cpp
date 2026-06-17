#include "../../includes/server/Channel.hpp"

Channel::Channel(const std::string &name) : _name(name) {}

void    Channel::addClient(int clientFd)
{
    _clients.insert(clientFd);
}

void    Channel::removeClient(int clientFd) 
{
    _clients.erase(clientFd);
    _operators.erase(clientFd);
}

void    Channel::addOperator(int clientFd) 
{
    if (_clients.count(clientFd))
        _operators.insert(clientFd);
}

bool    Channel::isMember(int clientFd) const 
{
    return (_clients.count(clientFd));
}

bool    Channel::isOperator(int clientFd) const 
{
    return (_operators.count(clientFd));
}

const std::set<int> &Channel::getClients() const 
{
    return _clients;
}