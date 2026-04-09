#ifndef PASS_HPP
#define PASS_HPP

# include "../client/Client.hpp"
# include <vector>

bool	auth_pass(std::vector<std::string> split_msg, Client &client, std::string password);

#endif
