#include "../../includes/client/Client.hpp"
#include <iostream>

bool	authPass(Client &client, std::string password, std::string server_password)
{
	if (client.getIsAuthenticated())
	{
		std::cout << "Client already authenticated" << std::endl;
		return (true);
	}
	else if (server_password.compare(password) == 0)
	{
		std::cout << "Password accepted" << std::endl;
		client.setIsAuthenticated(true);
		return (true);
	}
		std::cout << "Wrong password" << std::endl;
	return (false);
}
