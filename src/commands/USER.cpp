#include "../../includes/utils/Utils.hpp"
#include <cstddef>
#include <iostream>
#include <string>

bool	parseUsername(std::string username)
{
	size_t	found;

	if (username.empty() || username.size() > 32)
	{
		std::cout << "Username empty or too large" << std::endl;
		return (false);
	}
	found = username.find_first_of(" @!:\r\n\0"); 
	if (found != std::string::npos)
	{
		std::cout << "Character " << username[found] << " is forbidden" << std::endl;
		return false;
	}
	std::cout << "username accepted -> " << username << std::endl;
	return (true);
}

bool	parseRealName(std::vector<std::string> subvec)
{
	for (std::vector<std::string>::iterator it = subvec.begin(); it != subvec.end(); ++it) {
		std::cout << *it << std::endl;
	}
	return (true);
}

bool	fetchRealName(std::string message, Client &client)
{
	size_t	colonPos;
	size_t	found;
	std::string	realName;

	colonPos = message.find_first_of(':') + 1;
	realName = message.substr(colonPos, message.length());
	realName.erase(realName.length() - 1);
	found = realName.find_first_of("\r\n\0");
	if (found != std::string::npos)
	{
		std::cout << "Invalid real name" << std::endl;	
		return (false);
	}
	client.setRealname(realName);
	return (true);
}

bool	setClientUsername(std::string message, std::vector<std::string> split_msg, Client &client)
{
	std::string realname;

	if (!client.getIsAuthenticated())
	{
		std::cout << "Client not authenticated" << std::endl;
		return (false);
	}
	else
	{
		if (split_msg.size() < 5)
		{
			std::cout << "Not enough parameters" << std::endl;
			return false;
		}
		for (std::vector<std::string>::iterator it = split_msg.begin(); it != split_msg.end(); ++it)
		{
			int index = it - split_msg.begin();

			switch (index) {
				case 1:
					if (!parseUsername(*it))
						return (false);
					else
						client.setUsername(*it);
					break ;
				case 2:
					if (it->compare("0") != 0)
					{
						std::cout << "Invalid mode " << *it << std::endl;
						return (false);
					}
					break ;
				case 3:
					if (it->compare("*") != 0)
					{
						std::cout << "This unused " << *it << " is invalid" << std::endl;	
						return (false);
					}
					break ;
			}
		}
		if (!fetchRealName(message, client))
			return (false);
	}
	return (true);
}
