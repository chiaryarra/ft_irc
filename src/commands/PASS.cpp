#include "../../includes/client/Client.hpp"
#include "../../includes/utils/Utils.hpp"
#include <iostream>

std::string	authPass(Client &client, std::string password, std::string server_password)
{
	if (client.getIsAuthenticated())
		return ERR_ALREADYREGISTRED;
	if (server_password.compare(password) != 0)
		return (ERR_PASSWDMISMATCH);
	client.setIsAuthenticated(true);
		return (RPL_SUCCESS);
}
