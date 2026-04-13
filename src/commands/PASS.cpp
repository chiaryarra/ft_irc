#include "../../includes/client/Client.hpp"
#include <vector>
#include <iostream>

bool	auth_pass(std::vector<std::string> split_msg, Client &client, std::string password)
{
	if (client.getIsAuthenticated())
	{
		std::cout << "Client already authenticated" << std::endl;
		return (true);
	}
	else if (split_msg[1].compare(password) == 0)
	{
		std::cout << "Password accepted" << std::endl;
		client.setIsAuthenticated(true);
		return (true);
	}
		std::cout << "Wrong password" << std::endl;
	return (false);
}
