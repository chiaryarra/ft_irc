#include "../../includes/client/Client.hpp"
#include <vector>
#include <iostream>

bool	auth_pass(std::vector<std::string> split_msg, Client &client, std::string password)
{
	if (split_msg[1].empty())
		std::cout << "Pass needed" << std::endl;
	else	
	{
		if (split_msg[1].compare(password) == 0)
		{
			std::cout << "Password accepted" << std::endl;
			client.setIsAuthenticated(true);
			return (true);
		}
			std::cout << "Wrong password" << std::endl;
	}
	return (false);
}
