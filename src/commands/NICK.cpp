#include "../../includes/utils/Utils.hpp"
#include <cctype>

bool	isSpecial(char c)
{
	std::string	special = "[]\\`^{}|-";
	
	if (special.find(c) != std::string::npos)
		return (true);
	return (false);
}

bool	isNewNick(std::map<int, Client> &clients, std::string nickname)
{
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		Client client = it->second;
		if (client.getNickname().compare(nickname) == 0)
			return (false);
	}
	return (true);
}

std::string	setClientNick(std::string nickname, Client &client, std::map<int, Client> &clients)
{
	if (nickname.length() < 1 || nickname.length() > 9)
	{
		std::cout << "Ivalid nick: nickname must be between 1 and 9. Client FD: " << client.getFd() << std::endl;
		return (ERR_ERRONEUSNICKNAME);
	}
	if (!std::isalpha(nickname[0]) && !isSpecial(nickname[0]))
	{
		std::cout << "Ivalid nick: first character must be alpha or special. Client FD: " << client.getFd() << std::endl;
		return (ERR_ERRONEUSNICKNAME);
	}
	if (!isNewNick(clients, nickname))
	{
		std::cout << "Nick " << nickname << " has already been registered. Client FD: " << client.getFd() << std::endl;
		return (ERR_NICKNAMEINUSE);
	}
	for (std::string::iterator it = nickname.begin(); it != nickname.end(); ++it)
	{
		if (!std::isalpha(*it) && !std::isdigit(*it) && !isSpecial(*it))
		{
			std::cout << "Ivalid nick: invalid character: " << *it << ". Client FD: " << client.getFd() << std::endl;
			return (ERR_ERRONEUSNICKNAME);
		}
	}
	client.setNickname(nickname);
	std::cout << "nick set: " << client.getNickname() << std::endl;
	return (RPL_SUCCESS);
}
