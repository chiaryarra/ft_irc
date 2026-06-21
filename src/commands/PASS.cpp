#include "../../includes/client/Client.hpp"
#include "../../includes/utils/Utils.hpp"
#include <iostream>

std::string	authPass(Client &client, std::string password, std::string server_password)
{
	if (client.getIsAuthenticated())
	{
		std::cout << "Client FD: " << client.getFd() << " already authenticated" << std::endl;
		return ERR_ALREADYREGISTRED;
	}
	if (server_password.compare(password) != 0)
	{
		std::cout << "Wrong password from FD: " << client.getFd() << std::endl;
		return (ERR_PASSWDMISMATCH);
	}
		std::cout << "Password accepted from FD: " << client.getFd() << std::endl;
		client.setIsAuthenticated(true);
		return (RPL_SUCCESS);
}
