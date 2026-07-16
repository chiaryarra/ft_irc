#include "../../includes/utils/Utils.hpp"

std::string inviteUser(Client &sender, Client &target, Channel &channel)
{
	if (!channel.isMember(sender.getFd()))
		return ERR_NOTONCHANNEL;
	if (channel.isInviteOnly() && !channel.isOperator(sender.getFd()))
		return ERR_CHANOPRIVSNEEDED;
	channel.addInvite(target.getFd());
	return RPL_INVITING;
}
