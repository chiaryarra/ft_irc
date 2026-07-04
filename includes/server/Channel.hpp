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
    void addClient(int clientFd);
    void removeClient(int clientFd);
    void addOperator(int clientFd);
    bool isMember(int clientFd) const;
    bool isOperator(int clientFd) const;
	
	const std::set<int> &getClients() const;

	void	setTopic(std::string &topic);
	void	setMode(std::string &modes);
	void	setKey(std::string &key);
	void	setUserLimit(int limit);


	const std::string	&getTopic() const;
	const std::string	&getMode() const;
	const std::string	&getKey() const;
	const int			&getUserLimit() const;

	bool	hasMode();
	bool	isInvited();

	void	removeMode(std::string modes);
	void	removeInvite(int clientFd);

	void	addInvite(int clientFd);

	

};

#endif
