#include "../../includes/client/Client.hpp"
#include <iostream>

bool	authPass(Client &client, std::string password, std::string server_password)
{
	if (server_password.compare(password) == 0)
	{
		std::cout << "Password accepted from FD: " << client.getFd() << std::endl;
		client.setIsAuthenticated(true);
		return (true);
	}
		std::cout << "Wrong password from FD: " << client.getFd() << std::endl;
	return (false);
}
