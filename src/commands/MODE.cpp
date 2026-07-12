#include "../../includes/utils/Utils.hpp"
#include <sstream>

bool	needParam(char op, char mode)
{
	if (op == '+' && (mode == 'k' || mode == 'o' || mode == 'l'))
		return true;
	if (op == '-' && mode == 'o')
		return true;
	return false;
}

bool	isValidMode(char mode)
{
	if (mode != '+' && mode != '-')
		return true;
	return false;
}

void	addKey(Channel &channel, std::string key)
{
	channel.setKey(key);
}

void	addOperator(Channel &channel, Client client)
{
	channel.addOperator(client.getFd());
}

void	addUserLimit(Channel &channel, unsigned int limit)
{
	channel.setUserLimit(limit);
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
	unsigned int						limit;
	std::string							validModes;
	std::stringstream					iss;
	std::map<int, Client>::iterator		clientIt;
	std::vector<std::string>::iterator	paramIt;

	paramIt = params.begin();
	validModes = "+-itkol";
	if (modes.empty() && params.size() == 0)
		return RPL_CHANNELMODEIS;
	if (!isMember)
		return ERR_NOTONCHANNEL;

	if (isOperator)
	{
		if (modes.find_first_not_of(validModes) != std::string::npos)
			return ERR_UNKNOWNMODE;
		if (modes.at(0) != '+' && modes.at(0) != '-')
			return ERR_NEEDMOREPARAMS;
		for (std::string::iterator it = modes.begin(); it != modes.end(); ++it)
		{
			std::string::iterator next = it;
			++next;
			if (next == modes.end())
				continue;
			if (*it == '+' && isValidMode(*next))
			{
				if (needParam(*it, *next))
				{
					if (paramIt->size() == 0 || paramIt == params.end())
						return ERR_NEEDMOREPARAMS;
					switch (*next) {
						case 'k':
							addKey(channel, *paramIt);
							++paramIt;
							break;
						case 'o':
							clientIt = findClientByNick(clients, *paramIt);
							if (clientIt == clients.end())
								return ERR_NOSUCHNICK;
							if (!channel.isMember(clientIt->second.getFd()))
								return ERR_USERNOTINCHANNEL;
							addOperator(channel, clientIt->second);
							++paramIt;
							break;
						case 'l':
							iss.clear();
							iss.str(*paramIt);
							if (iss >> limit && iss.eof() && paramIt->at(0) != '-')
							{
								addUserLimit(channel, limit);
								++paramIt;
							}
							else
								return ERR_UNKNOWNMODE;
							break;
						default:
							return ERR_UNKNOWNMODE;
					}
					channel.addMode(*next);
				}
				else
				{
					channel.addMode(*next);
				}	
			}
			else
				return ERR_UNKNOWNMODE;
		}
	}
	else
		return ERR_CHANOPRIVSNEEDED;
	return RPL_SUCCESS;
}
