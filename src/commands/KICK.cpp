#include "../../includes/utils/Utils.hpp"

std::string	manangeKickCommand(Client &client, Channel &channel, Client &target)
{
	if (!channel.isMember(client.getFd()))
		return ERR_NOTONCHANNEL;
	if (!channel.isOperator(client.getFd()))
		return ERR_CHANOPRIVSNEEDED;
	if (!channel.isMember(target.getFd()))
		return ERR_USERNOTINCHANNEL;
	return RPL_SUCCESS;
}
