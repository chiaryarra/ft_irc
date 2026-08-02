#include "../../includes/utils/Utils.hpp"
#include <cstddef>
#include <iostream>
#include <string>

bool	parseUsername(std::string username, Client &client)
{
	size_t	found;

	if (username.empty() || username.size() > 32)
		return (false);
	found = username.find_first_of(" @!:\r\n\0"); 
	if (found != std::string::npos)
		return false;
	return (true);
}

bool	parseRealName(std::string realname, Client &client)
{
	if (realname.find_first_of("\r\n\0") != std::string::npos)
		return (false);
	client.setRealname(realname);
	return (true);
}

std::string	setClientUsername(std::vector<std::string> split_msg, Client &client)
{
	for (std::vector<std::string>::iterator it = split_msg.begin(); it != split_msg.end(); ++it)
	{
		int index = it - split_msg.begin();
		switch (index) {
			case 1:
				if (!parseUsername(*it, client))
					return (ERR_INVALIDUSERNAME);
				else
					client.setUsername(*it);
				break ;
			case 2:
				if (it->compare("0") != 0)
				{
					client.setUsername("");					
					return (ERR_INVALIDMODE);
				}
				break ;
			case 3:
				if (it->compare("*") != 0)
				{
					client.setUsername("");					
					return (ERR_INVALIDUNUSED);
				}
				break ;
			case 4:
				if (!parseRealName(*it, client))
					return (ERR_INVALIDREALNAME);
				else
					client.setRealname(*it);
		}
	}
	return (RPL_SUCCESS);
}
