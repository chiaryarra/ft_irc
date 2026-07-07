#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>

class Channel
{
private:
    std::string		_name;
    std::set<int>	_clients;
    std::set<int>	_operators;
	std::string		_topic;
	std::string		_modes;
	std::string		_key;
	std::set<int>	_inviteList;
	int				_userLimit;
public:
    Channel(const std::string &name);

	const std::set<int> &getClients() const;
	const std::string	&getTopic() const;
	const std::string	&getModes() const;
	const std::string	&getKey() const;
	const int			&getUserLimit() const;
	
	void	setTopic(std::string &topic);
	void	setModes(std::string &modes);
	void	setKey(std::string &key);
	void	setUserLimit(int limit);

	void	addClient(int clientFd);
    void	addOperator(int clientFd);
	void	addInvite(int clientFd);
	void	addMode(char mode);

	bool	isMember(int clientFd) const;
    bool	isOperator(int clientFd) const;
	bool	isInvited(int clientFd) const;
	bool	hasMode(char mode) const;

	void	removeMode(char mode);
	void	removeInvite(int clientFd);
    void	removeClient(int clientFd);
	void	removeOperator(int clientFd);
};

#endif
