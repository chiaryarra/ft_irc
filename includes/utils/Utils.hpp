#ifndef UTILS_HPP
#define UTILS_HPP

# include "../client/Client.hpp"
# include <iostream>

bool	authPass(Client &client, std::string password, std::string server_password);
void	setClientNick(std::string nickname, Client &client);

#endif
