#include "../../includes/server/Server.hpp"
#include <iostream>
#include <cstdlib>

int parsePort(const std::string& input)
{
    if (input.empty() == true)
        throw std::invalid_argument("Port is empty");
    for(size_t i = 0; i < input.length(); i++)
    {
        if (std::isdigit(input[i]) == 0)
            throw std::invalid_argument("Port must be only numeric");
    }
    long port = std::atol(input.c_str());
    if (port < 1024 || port > 65535)
        throw std::out_of_range("Port out of valid range (1024-65535)");
    return (static_cast<int>(port));
}

int main(int argc, char **argv) {
    if (argc != 3)
    {
        std::cerr << "Correct usage: ./ircserv <port> <password>" << std::endl;
        return (1);
    }
    try {
        int port = parsePort(argv[1]);
        std::string password = argv[2];
        Server server(port, password);
        server.start();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return (1);
    }

    return (0);
}