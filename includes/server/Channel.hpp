#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <set>
#include <string>

class Channel
{
  private:
	std::string _name;
	std::set<int> _clients;
	std::set<int> _operators;
	std::string _topic;
	std::string _modes;
	std::string _key;
	std::set<int> _inviteList;
	unsigned int _userLimit;
	bool _inviteOnly;
	bool _topicProtected;

  public:
	Channel();
	Channel(const std::string &name);
	Channel &operator=(const Channel &other);
	~Channel();

	const std::string &getName() const;
	const std::set<int> &getClients() const;
	const std::set<int> &getOperators() const;
	const std::string &getTopic() const;
	const std::string &getModes() const;
	const std::string &getKey() const;
	const unsigned int &getUserLimit() const;
	const bool &isInviteOnly() const;
	const bool &isTopicProtected() const;
	const std::string getUserLimitStr() const;

	void setTopic(std::string topic);
	void setModes(std::string &modes);
	void setKey(std::string key);
	void setUserLimit(unsigned int limit);
	void setInviteOnly(bool mode);
	void setTopicProtected(bool mode);

	void addClient(int clientFd);
	void addOperator(int clientFd);
	void addInvite(int clientFd);
	bool addMode(char mode);

	bool isMember(int clientFd) const;
	bool isOperator(int clientFd) const;
	bool isInvited(int clientFd) const;
	bool hasMode(char mode) const;

	bool removeMode(char mode);
	void removeInvite(int clientFd);
	void removeClient(int clientFd);
	void removeOperator(int clientFd);
};

#endif
