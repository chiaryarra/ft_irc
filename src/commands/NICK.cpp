#include "../../includes/utils/Utils.hpp"
#include <cctype>

bool	isSpecial(char c)
{
	std::string	special = "[]\\`^{}|-";
	
	if (special.find(c) != std::string::npos)
		return (true);
	return (false);
}

void	setClientNick(std::string nickname, Client &client)
{
	if (client.getIsAuthenticated())
	{
		if (nickname.length() < 1 || nickname.length() > 9)
		{
			std::cout << "Ivalid nick: nickname must be between 1 and 9" << std::endl;
			return ;
		}
		if (!std::isalpha(nickname[0]) && !isSpecial(nickname[0]))
		{
			std::cout << "Ivalid nick: first character must be alpha or special" << std::endl;
			return ;
		}
		for (std::string::iterator it = nickname.begin(); it != nickname.end(); ++it)
		{
			if (!std::isalpha(*it) && !std::isdigit(*it) && !isSpecial(*it))
			{
				std::cout << "Ivalid nick: invalid character: " << *it << std::endl;
				return ;
			}
		}
		client.setNickname(nickname);
		std::cout << "nick set: " << client.getNickname() << std::endl;
	}
	else
		std::cout << "Client not authenticated" << std::endl;
}
