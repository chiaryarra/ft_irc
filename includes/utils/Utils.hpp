#ifndef UTILS_HPP
#define UTILS_HPP

# include <iostream>
# include <vector>
# include "../client/Client.hpp"

bool	authPass(Client &client, std::string password, std::string server_password);
void	setClientNick(std::string nickname, Client &client);
bool	auth_pass(std::vector<std::string> split_msg, Client &client, std::string password);

#endif
