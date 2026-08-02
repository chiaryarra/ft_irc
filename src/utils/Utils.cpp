#include "../../includes/utils/Utils.hpp"
#include <sstream>

std::map<int, Client>::iterator findClientByNick(std::map<int, Client> &clients, const std::string &nick)
{
	std::map<int, Client>::iterator it;

	for (it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second.getNickname().compare(nick) == 0)
			break;
	}
	return it;
}

std::vector<std::string> split(std::string message)
{
	std::vector<std::string>	res;
	std::string::size_type		trailing_pos;
	std::istringstream			iss;
	std::string					word;
	std::string					trailing;

	trailing_pos = message.find_first_of(':');
	if (trailing_pos != std::string::npos && trailing_pos > 1 && message.at(trailing_pos - 1) == ' ')
	{
		trailing = message.substr(trailing_pos + 1, message.size() - trailing_pos);
		message.resize(trailing_pos);
	}
	iss.str(message);
	while (iss >> word)
		res.push_back(word);
	if (!trailing.empty())
		res.push_back(trailing);
	return (res);
}
