#include "../../includes/utils/Utils.hpp"
#include <sstream>
#include <vector>

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

void	addOperator(Channel &channel, std::string key)
{
	(void)channel;
	(void)key;
}

void	addUserLimit(Channel &channel, unsigned int limit)
{
	channel.setUserLimit(limit);
}

std::string	manageChannelMode(Channel &channel, std::string modes, std::vector<std::string> &params, bool isMember, bool isOperator)
{
	std::vector<std::string>::iterator paramIt;
	std::string	validModes = "+-itkol";
	unsigned int	limit;
	std::stringstream iss;

	paramIt = params.begin();
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
							addOperator(channel, *paramIt);
							break;
						case 'l':

							iss.clear();
							iss.str(*paramIt);
							if (iss >> limit && iss.eof() && paramIt->at(0) != '-')
								addUserLimit(channel, limit);
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
