#ifndef UTILS_HPP
#define UTILS_HPP

# include "../client/Client.hpp"
# include <string>

bool	authPass(Client &client, std::string password, std::string server_password);

#endif
