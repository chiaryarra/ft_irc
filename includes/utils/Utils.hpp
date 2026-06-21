#ifndef UTILS_HPP
#define UTILS_HPP

# include <iostream>
# include <vector>
# include <string>
# include "../client/Client.hpp"

const std::string	RPL_SUCCESS = "000";
const std::string	RPL_WELCOME = "001";
const std::string	RPL_YOURHOST = "002";
const std::string	RPL_CREATED = "003";
const std::string	RPL_MYINFO = "004";

const std::string	ERR_UNKNOWNCOMMAND = "421";
const std::string	ERR_NONICKNAMEGIVEN = "431";
const std::string	ERR_ERRONEUSNICKNAME = "432";
const std::string	ERR_NICKNAMEINUSE = "433";
const std::string	ERR_NOTREGISTERED = "451";
const std::string	ERR_NEEDMOREPARAMS = "461";
const std::string	ERR_ALREADYREGISTRED = "462";
const std::string	ERR_PASSWDMISMATCH = "464";

const std::string	MSG_NEEDMOREPARAMS = ":Not enough parameters";


std::string	authPass(Client &client, std::string password, std::string server_password);
void	setClientNick(std::string nickname, Client &client);
bool	setClientUsername(std::string message, std::vector<std::string> split_msg, Client &client);

#endif
