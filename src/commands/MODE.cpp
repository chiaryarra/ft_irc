#include "../../includes/utils/Utils.hpp"
#include <iostream>
#include <sstream>

void	solveInviteMode(Channel &channel, char mode, bool isAddMode)
{
	if (isAddMode)
	{
		channel.setInviteOnly(true);
		channel.addMode(mode);
	}
	else 
	{
		channel.setInviteOnly(false);
		channel.removeMode(mode);
	}
}

void	solveTopicMode(Channel &channel, char mode, bool isAddMode)
{
	if (isAddMode)
	{
		channel.setTopicProtected(true);
		channel.addMode(mode);
	}
	else
	{
		channel.setTopicProtected(false);
		channel.removeMode(mode);
	}
}

void	solveKeyMode(Channel &channel, char mode, std::string key, bool isAddMode)
{
	if (isAddMode)
	{
		channel.setKey(key);
		channel.addMode(mode);
	}
	else 
	{
		channel.setKey("");
		channel.removeMode(mode);
	}
}

void	solveOperatorMode(Channel &channel, Client client, bool isAddMode)
{
	isAddMode ? channel.addOperator(client.getFd()) : channel.removeOperator(client.getFd());
}

void	removeOperator(Channel &channel, Client client)
{
	channel.removeOperator(client.getFd());
}

void	solveLimitMode(Channel &channel, char mode, unsigned int limit, bool isAddMode)
{
	if (isAddMode)
	{
		channel.setUserLimit(limit);
		channel.addMode(mode);
	}
	else
	{
		channel.setUserLimit(0);
		channel.removeMode(mode);
	}
}

std::map<int, Client>::iterator	findClientByNick(std::map<int, Client> &clients, std::string &nick)
{
	std::map<int, Client>::iterator it;

	for (it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second.getNickname().compare(nick) == 0)
			break;
	}
	return it;
}

std::string	manageChannelMode(Channel &channel, std::string modes, std::vector<std::string> &params, std::map<int, Client> &clients, bool isMember, bool isOperator)
{
	bool								addMode;
	unsigned int						limit;
	std::string							validModes;
	std::stringstream					iss;
	std::map<int, Client>::iterator		clientIt;
	std::vector<std::string>::iterator	paramIt;

	addMode = false;
	limit = 0;
	paramIt = params.begin();
	validModes = "itkol";
	if (modes.empty() && params.size() == 0)
		return RPL_CHANNELMODEIS;
	if (!isMember)
		return ERR_NOTONCHANNEL;

	if (isOperator)
	{
		if (modes.at(0) != '+' && modes.at(0) != '-')
			return ERR_UNKNOWNMODE + " " + modes.at(0);
		for (std::string::iterator it = modes.begin(); it != modes.end(); ++it)
		{
			if (*it == '+')
			{
				addMode = true;
				continue;
			}
			if (*it == '-')
			{
				addMode = false;
				continue;
			}
			if (validModes.find_first_of(*it) != std::string::npos)
			{
				switch (*it)
				{
					case 'i':
						solveInviteMode(channel, *it, addMode);
						break;
					case 't':
						solveTopicMode(channel, *it, addMode);
						break;
					case 'k':
						if (params.size() == 0 || paramIt == params.end())
							return ERR_NEEDMOREPARAMS;
						solveKeyMode(channel, *it, *paramIt, addMode);
						++paramIt;
						break;
					case 'o':
						if (params.size() == 0 || paramIt == params.end())
							return ERR_NEEDMOREPARAMS;
						clientIt = findClientByNick(clients, *paramIt);
						if (clientIt == clients.end())
							return ERR_NOSUCHNICK;
						if (!channel.isMember(clientIt->second.getFd()))
							return ERR_USERNOTINCHANNEL;
						solveOperatorMode(channel, clientIt->second, addMode);
						++paramIt;
						break;
					case 'l':
						if (addMode && (params.size() == 0 || paramIt == params.end()))
							return ERR_NEEDMOREPARAMS;
						else if (addMode)
						{
							iss.clear();
							iss.str(*paramIt);
							iss >> limit;
							if (!iss.eof() || paramIt->at(0) == '-')
								return ERR_UNKNOWNERROR;
						}
						solveLimitMode(channel, *it, limit, addMode);
						++paramIt;
						break;
					default:
						return ERR_UNKNOWNMODE + " " + *it;
				}
			}
			else
				return ERR_UNKNOWNMODE + " " + *it;
		}
	}
	else
		return ERR_CHANOPRIVSNEEDED;
	return RPL_SUCCESS;
}
