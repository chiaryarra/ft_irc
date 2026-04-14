#include "../../includes/utils/Utils.hpp"
#include <cstdlib>
#include <string>

bool	getRandAuth()
{
	std::srand(time(0));

	if (std::rand() % 2 == 0)
		return (true);
	return (false);
}

void	setClientNick(std::string nickname, Client &client)
{
	if (getRandAuth())
		client.setIsAuthenticated(true);
	if (client.getIsAuthenticated())
	{
		client.setNickname(nickname);
		std::cout << "nick set: " << client.getNickname() << std::endl;
	}
	else
		std::cout << "Client not authenticated" << std::endl;
}
