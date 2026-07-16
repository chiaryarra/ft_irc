#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <poll.h>
#include <string>
#include <vector>

#include "../client/Client.hpp"
#include "Channel.hpp"

class Server
{
  private:
	// Server atributes
	const std::string _serverName;
	const std::string _version;
	const std::string _creationDate;
	int _port;
	std::string _password;
	int _serverSocketFd;
	bool _running;
	std::map<int, Client> _clients;
	std::vector<pollfd> _pollFds;
	std::map<std::string, Channel> _channels;
	typedef void (Server::*CommandHandler)(Client &, const std::string &, const std::vector<std::string> &);
	std::map<std::string, CommandHandler> _cmdMap;
	std::map<std::string, std::string> _errorDescriptions;

	// Server private methods
	void setupSocketOpts();
	void setupServerAddress();
	void setupListen();
	void setupSocket();
	void setupNonBlocking();
	void setupServerPoll();
	void setupPolling();
	void runPollLoop();
	void handleNewConnection();
	void handleClientData(int clientFd);
	void processClientBuffer(Client &client);
	void removeClient(int clientFd);
	void sendMessage(int clientFd, const std::string &message);
	void sendWelcomeMessage(Client &client);
	void broadcastToChannel(const std::string &channelName, const std::string &message, int excludeFd);
	void initCommandMap();
	void initErrorDescriptions();
	void handlePass(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens);
	void handleNick(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens);
	void handleUser(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens);
	void handleJoin(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens);
	void handleCap(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens);
	void handlePing(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens);
	void handleMode(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens);
	void handleInvite(Client &client, const std::string &rawMsg, const std::vector<std::string> &tokens);
	void sendJoinMessage(Client &client, Channel &channel);
	void sendError(Client &client, const std::string &command, const std::string &errorCode, const std::string &extra);
	std::string showClientsInChannel(Channel &channel);
	std::string showChannelModes(Channel &channel);

  public:
	// Constructors
	Server(int port, const std::string &password);
	~Server();

	// Server public method
	void start();
};

#endif
