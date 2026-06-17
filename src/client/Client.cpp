#include "../../includes/client/Client.hpp"
#include <unistd.h>

Client::Client(int fd): _fd(fd), _nickname(""), _username(""), _inputBuffer(""), _isAuthenticated(false), _isRegistered(false) {}

Client::~Client() {}

int Client::getFd() const {
    return (_fd);
}

const std::string& Client::getNickname() const {
    return (_nickname);
}

const std::string& Client::getUsername() const {
    return (_username);
}

const std::string& Client::getRealname() const {
	return (_realname);
}

std::string& Client::getInputBuffer() {
    return (_inputBuffer);
}

bool    Client::getIsAuthenticated() const {
    return (_isAuthenticated);
}

bool	Client::getIsRegistered() const {
	return (_isRegistered);
}

void    Client::setNickname(const std::string& nickname) {
    _nickname = nickname;
}

void    Client::setUsername(const std::string& username) {
    _username = username;
}

void    Client::setRealname(const std::string& realname) {
    _realname = realname;
}

void    Client::setIsAuthenticated(bool state) {
    _isAuthenticated = state;
}

void	Client::setIsRegistered(bool state) {
	_isRegistered = state;
}

void    Client::appendBuffer(const std::string& append) {
    _inputBuffer += append;
}

void    Client::clearBuffer() {
    _inputBuffer.clear();
}
