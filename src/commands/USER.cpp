#include "../../includes/utils/Utils.hpp"
#include <cstddef>
#include <iostream>
#include <string>

bool	parseUsername(std::string username, Client &client)
{
	size_t	found;

	if (username.empty() || username.size() > 32)
	{
		std::cout << "Username empty or too large. Client FD: " << client.getFd() << std::endl;
		return (false);
	}
	found = username.find_first_of(" @!:\r\n\0"); 
	if (found != std::string::npos)
	{
		std::cout << "Character " << username[found] << " is forbidden. Client FD:" << client.getFd() << std::endl;
		return false;
	}
	std::cout << "username accepted -> " << username << ". Client FD: " << client.getFd() << std::endl;
	return (true);
}

bool	fetchRealName(std::string message, Client &client)
{
	size_t	colonPos;
	size_t	found;
	std::string	realName;

	colonPos = message.find_first_of(':') + 1;
	realName = message.substr(colonPos, message.length());
	realName.erase(realName.length());
	found = realName.find_first_of("\r\n\0");
	if (found != std::string::npos)
	{
		std::cout << "Invalid real name. Client FD: " << client.getFd() << std::endl;	
		return (false);
	}
	client.setRealname(realName);
	return (true);
}

std::string	setClientUsername(std::string message, std::vector<std::string> split_msg, Client &client)
{
	std::string realname;

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
					std::cout << "Invalid mode " << *it << std::endl;
					return (ERR_INVALIDMODE);
				}
				break ;
			case 3:
				if (it->compare("*") != 0)
				{
					std::cout << "This unused " << *it << " is invalid" << std::endl;	
					return (ERR_INVALIDUNUSED);
				}
				break ;
		}
	}
	if (!fetchRealName(message, client))
		return (ERR_INVALIDREALNAME);
	return (RPL_SUCCESS);
}
