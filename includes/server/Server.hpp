#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <poll.h>

class Client;

class Server
{
private:
    // Server atributes
    int _port;
    std::string _password;
    int _serverSocketFd;
    bool    _running;
    std::map<int, Client>  _clients; 
    std::vector<pollfd> _pollFds;

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
public:
    // Constructors
    Server(int port, const std::string& password);
    ~Server();
    
    // Server public method
    void    start();
};

#endif