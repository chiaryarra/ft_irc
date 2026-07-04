#ifndef UTILS_HPP
#define UTILS_HPP

#include "../client/Client.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>

/* Welcome reply */
const std::string	RPL_SUCCESS = "000";
const std::string	RPL_WELCOME = "001";
const std::string	RPL_YOURHOST = "002";
const std::string	RPL_CREATED = "003";
const std::string	RPL_MYINFO = "004";

/* Register error replies */
const std::string	ERR_UNKNOWNCOMMAND = "421";
const std::string	ERR_NONICKNAMEGIVEN = "431";
const std::string	ERR_ERRONEUSNICKNAME = "432";
const std::string	ERR_NICKNAMEINUSE = "433";
const std::string	ERR_NOTREGISTERED = "451";
const std::string	ERR_NEEDMOREPARAMS = "461";
const std::string	ERR_ALREADYREGISTRED = "462";
const std::string	ERR_PASSWDMISMATCH = "464";

/* Extra register error replies */
const std::string	ERR_INVALIDUSERNAME = "900";
const std::string	ERR_INVALIDMODE = "901";
const std::string	ERR_INVALIDUNUSED = "902";
const std::string	ERR_INVALIDREALNAME = "903";

/* Channel success replies */
const std::string	RPL_TOPIC = "332";
const std::string	RPL_INVITING = "341";
const std::string	RPL_NAMREPLY = "353";
const std::string	RPL_ENDOFNAMES = "366";
const std::string	RPL_CHANNELMODEIS = "324";

/* Channel error replies */
const std::string	RR_NOSUCHNICK = "401";
const std::string	ERR_NOSUCHCHANNEL = "403";
const std::string	ERR_CANNOTSENDTOCHAN = "404";
const std::string	ERR_TOOMANYCHANNELS = "405";
const std::string	ERR_USERNOTINCHANNEL = "441";
const std::string	ERR_NOTONCHANNEL = "442";
const std::string	ERR_KEYSET = "467";
const std::string	ERR_CHANNELISFULL = "471";
const std::string	ERR_UNKNOWNMODE = "472";
const std::string	ERR_INVITEONLYCHAN = "473";
const std::string	ERR_BANNEDFROMCHAN = "474";
const std::string	ERR_BADCHANNELKEY = "475";
const std::string	ERR_CHANOPRIVSNEEDED = "482";

const std::string	MSG_NEEDMOREPARAMS = ":Not enough parameters";

std::string authPass(Client &client, std::string password, std::string server_password);
std::string setClientNick(std::string nickname, Client &client, std::map<int, Client> &clients);
std::string setClientUsername(std::string message, std::vector<std::string> split_msg, Client &client);

#endif
