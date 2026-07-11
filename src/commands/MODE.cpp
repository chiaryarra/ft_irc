#include "../../includes/utils/Utils.hpp"
#include <vector>

std::string	manageChannelMode(const Channel &channel, const std::string modes, const std::vector<std::string> params)
{
	(void)channel;
	if (modes.empty() && params.size() == 0)
		return RPL_CHANNELMODEIS;
	return RPL_SUCCESS;
}
