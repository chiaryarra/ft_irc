#include "../../includes/utils/Utils.hpp"
#include <cctype>

bool	isSpecial(char c)
{
	std::string	special = "[]\\`^{}|-";
	
	if (special.find(c) != std::string::npos)
		return (true);
	return (false);
}

bool	isNewNick(Client &client, std::map<int, Client> &clients, std::string nickname)
{
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second.getNickname().compare(nickname) == 0 && it->second.getFd() != client.getFd())
			return (false);
	}
	return (true);
}

std::string	setClientNick(std::string nickname, Client &client, std::map<int, Client> &clients)
{
	if (nickname.length() < 1 || nickname.length() > 9)
		return (ERR_ERRONEUSNICKNAME);
	if (!std::isalpha(nickname[0]) && !isSpecial(nickname[0]))
		return (ERR_ERRONEUSNICKNAME);
	if (!isNewNick(client, clients, nickname))
		return (ERR_NICKNAMEINUSE);
	for (std::string::iterator it = nickname.begin(); it != nickname.end(); ++it)
		if (!std::isalpha(*it) && !std::isdigit(*it) && !isSpecial(*it))
			return (ERR_ERRONEUSNICKNAME);
	if (client.getNickname().compare(nickname) != 0)
		client.setNickname(nickname);
	return (RPL_SUCCESS);
}
