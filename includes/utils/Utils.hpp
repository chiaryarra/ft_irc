#ifndef UTILS_HPP
#define UTILS_HPP

# include <iostream>
# include <vector>
# include <string>
# include "../client/Client.hpp"

bool	authPass(Client &client, std::string password, std::string server_password);
void	setClientNick(std::string nickname, Client &client);
bool	setClientUsername(std::string message, std::vector<std::string> split_msg, Client &client);

#endif
