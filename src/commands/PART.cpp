#include "../../includes/utils/Utils.hpp"

std::string	managePartCommand(Client &client, Channel &channel)
{
	if (!channel.isMember(client.getFd()))
		return ERR_NOTONCHANNEL;
	return RPL_SUCCESS;
}

