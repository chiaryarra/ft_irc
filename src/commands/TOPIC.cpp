#include "../../includes/utils/Utils.hpp"

std::string	manageChannelTopic(Client &client, Channel &channel, const std::vector<std::string> &tokens)
{
	if (tokens.size() == 2)
	{
		if (channel.getTopic().empty())
			return RPL_NOTOPIC;
		return RPL_TOPIC;
	}
	else
	{
		if (!channel.isMember(client.getFd()))
			return ERR_NOTONCHANNEL;
		if (channel.isTopicProtected())
		{
			if (channel.isOperator(client.getFd()))
				channel.setTopic(tokens[2]);
			else
				return ERR_CHANOPRIVSNEEDED;
		}
		else
			channel.setTopic(tokens[2]);
	}
	return RPL_SUCCESS;
}
