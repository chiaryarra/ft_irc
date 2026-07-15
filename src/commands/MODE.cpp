#include "../../includes/utils/Utils.hpp"
#include <iostream>
#include <sstream>

bool solveInviteMode(Channel &channel, char mode, bool isAddMode)
{
	if (isAddMode)
	{
		channel.setInviteOnly(true);
		return (channel.addMode(mode));
	}
	else
	{
		channel.setInviteOnly(false);
		return (channel.removeMode(mode));
	}
}

bool solveTopicMode(Channel &channel, char mode, bool isAddMode)
{
	if (isAddMode)
	{
		channel.setTopicProtected(true);
		return (channel.addMode(mode));
	}
	else
	{
		channel.setTopicProtected(false);
		return (channel.removeMode(mode));
	}
}

bool solveKeyMode(Channel &channel, char mode, std::string key, bool isAddMode)
{
	if (isAddMode && channel.getKey().empty())
	{
		channel.setKey(key);
		return (channel.addMode(mode));
	}
	else if (!isAddMode)
	{
		channel.setKey("");
		return (channel.removeMode(mode));
	}
	return false;
}

bool solveOperatorMode(Channel &channel, Client client, bool isAddMode)
{
	if (isAddMode)
	{
		if (!channel.isOperator(client.getFd()))
		{
			channel.addOperator(client.getFd());
			return true;
		}
	}
	else
	{
		if (channel.isOperator(client.getFd()))
		{
			channel.removeOperator(client.getFd());
			return true;
		}
	}
	return false;
}

bool solveLimitMode(Channel &channel, char mode, unsigned int limit, bool isAddMode)
{
	if (isAddMode)
	{
		if (channel.getUserLimit() != limit)
		{
			channel.setUserLimit(limit);
			channel.addMode(mode);
			return true;
		}
		return false;
	}
	channel.setUserLimit(0);
	return (channel.removeMode(mode));
}

std::map<int, Client>::iterator findClientByNick(std::map<int, Client> &clients, std::string &nick)
{
	std::map<int, Client>::iterator it;

	for (it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second.getNickname().compare(nick) == 0)
			break;
	}
	return it;
}

void appendModeChange(std::string &modeChange, char mode, bool addMode, bool &changeFlag)
{
	if (changeFlag)
	{
		modeChange += addMode ? '+' : '-';
		changeFlag = false;
	}
	modeChange += mode;
}

std::string manageChannelMode(Channel &channel, std::string modes, std::vector<std::string> &params, std::map<int, Client> &clients, bool isMember, bool isOperator, std::string &modeChange)
{
	bool addMode;
	unsigned int limit;
	std::string validModes;
	std::stringstream iss;
	std::map<int, Client>::iterator clientIt;
	std::vector<std::string>::iterator paramIt;
	std::string paramChange;

	limit = 0;
	paramIt = params.begin();
	validModes = "itkol";
	if (modes.empty() && params.size() == 0)
		return RPL_CHANNELMODEIS;
	if (!isMember)
		return ERR_NOTONCHANNEL;

	bool changeFlag;

	if (isOperator)
	{
		if (modes.at(0) != '+' && modes.at(0) != '-')
			return ERR_UNKNOWNMODE + " " + modes.at(0);
		addMode = modes.at(0) == '+' ? false : true;
		for (std::string::iterator it = modes.begin(); it != modes.end(); ++it)
		{
			if (*it == '+')
			{
				if (!addMode)
					changeFlag = true;
				addMode = true;
				continue;
			}
			if (*it == '-')
			{
				if (addMode)
					changeFlag = true;
				addMode = false;
				continue;
			}
			if (validModes.find_first_of(*it) != std::string::npos)
			{
				switch (*it)
				{
				case 'i':
					if (solveInviteMode(channel, *it, addMode))
						appendModeChange(modeChange, *it, addMode, changeFlag);
					break;
				case 't':
					if (solveTopicMode(channel, *it, addMode))
						appendModeChange(modeChange, *it, addMode, changeFlag);
					break;
				case 'k':
					if (params.size() == 0 || paramIt == params.end())
						return ERR_NEEDMOREPARAMS;
					if (solveKeyMode(channel, *it, *paramIt, addMode))
					{
						appendModeChange(modeChange, *it, addMode, changeFlag);
						paramChange += ' ' + *paramIt;
					}
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
					if (solveOperatorMode(channel, clientIt->second, addMode))
					{
						appendModeChange(modeChange, *it, addMode, changeFlag);
						paramChange += ' ' + *paramIt;
					}
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
						if (!iss.eof() || paramIt->at(0) == '-' || limit == 0)
							return ERR_UNKNOWNERROR;
					}
					if (solveLimitMode(channel, *it, limit, addMode))
					{
						appendModeChange(modeChange, *it, addMode, changeFlag);
						if (addMode)
							paramChange += ' ' + *paramIt;
					}
					if (addMode)
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
	modeChange += paramChange;
	return RPL_SUCCESS;
}
