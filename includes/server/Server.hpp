#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <poll.h>

#include "Channel.hpp"
#include "../client/Client.hpp"

class Server
{
private:
    // Server atributes
	const std::string	_serverName;
	const std::string	_version;
    int _port;
    std::string _password;
    int _serverSocketFd;
    bool    _running;
    std::map<int, Client>  _clients; 
    std::vector<pollfd> _pollFds;
    std::map<std::string, Channel> _channels;
	typedef void (Server::*CommandHandler)(Client&, const std::string&, const std::vector<std::string>&);
	std::map<std::string, CommandHandler> _cmdMap;

    // Server private methods
    void    setupSocketOpts();
    void    setupServerAddress();
    void    setupListen();
    void    setupSocket();
    void    setupNonBlocking();
    void    setupServerPoll();
    void    setupPolling();
    void    runPollLoop();
    void    handleNewConnection();
    void    handleClientData(int clientFd);
    void    processClientBuffer(Client &client);
    void    removeClient(int clientFd);
    void    sendMessage(int clientFd, const std::string &message);
    void    broadcastToChannel(const std::string &channelName, const std::string &message, int excludeFd);
	void	initCommandMap();
	void	handlePass(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens);
	void	handleNick(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens);
	void	handleUser(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens);
public:
    // Constructors
    Server(int port, const std::string& password);
    ~Server();
    
    // Server public method
    void    start();
};

#endif
