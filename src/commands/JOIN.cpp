#include "../../includes/utils/Utils.hpp"

std::string joinChannel(Client &client, std::string channelName, bool isNewChannel)
{
	std::string forbidden = " ,:\a";

	(void)client;
	(void)isNewChannel;
	if (!client.getIsRegistered())
		return ERR_NOTREGISTERED;
	if (channelName.at(0) != '#' || channelName.find_first_of(forbidden) != std::string::npos)
		return ERR_NOSUCHCHANNEL;
	return RPL_SUCCESS;
}
