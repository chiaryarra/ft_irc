#include "../../includes/utils/Utils.hpp"
#include <iostream>

bool	parseUsername(std::string username)
{
	std::cout << username << std::endl;
	return (true);
}

bool	parseRealName(std::vector<std::string> subvec)
{
	for (std::vector<std::string>::iterator it = subvec.begin(); it != subvec.end(); ++it) {
		std::cout << *it << std::endl;
	}
	return (true);
}

bool	setClientUsername(std::vector<std::string> split_msg)
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
			case 4:
				std::vector<std::string> subvec(split_msg.begin() + index, split_msg.end());
				if (!parseRealName(subvec))
					return (false);
		}
	}
	return (true);
}
