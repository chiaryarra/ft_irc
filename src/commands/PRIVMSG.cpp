#include "../../includes/utils/Utils.hpp"

std::string managePrivmsgToChannel(Client &client, Channel &channel, std::string msg)
{
  if (!channel.isMember(client.getFd()))
    return ERR_NOTONCHANNEL;
  if (msg.empty())
    return ERR_NOTEXTTOSEND; 
  
  return RPL_SUCCESS;
}

std::string managePrivmsgToClient(std::string msg)
{
  if (msg.empty())
    return ERR_NOTEXTTOSEND;
  return RPL_SUCCESS;
}
