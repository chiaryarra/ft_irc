#include "../../includes/utils/Utils.hpp"

std::map<int, Client>::iterator findClientByNick(std::map<int, Client> &clients, std::string &nick)
{
	std::map<int, Client>::iterator it;

	for (it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second.getNickname().compare(nick) == 0)
			break;
	}
	return it;
}
