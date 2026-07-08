#include "../../includes/server/Channel.hpp"
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

std::string checkChannelMode(Channel &channel, int clientFd, bool isKeyPass)
{
	if (channel.getModes().find_first_of("l") != std::string::npos)
		if (channel.getClients().size() >= channel.getUserLimit())
			return ERR_CHANNELISFULL;
	if (channel.getModes().find_first_of("i") != std::string::npos)
		if (!channel.isInvited(clientFd))
			return ERR_INVITEONLYCHAN;
	if (channel.getModes().find_first_of("k") != std::string::npos)
		if (!isKeyPass)
			return ERR_BADCHANNELKEY;
	return RPL_SUCCESS;
}
