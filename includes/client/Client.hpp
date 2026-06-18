#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
private:
    int _fd;
    std::string _nickname;
    std::string _username;
	std::string _realname;
    std::string _inputBuffer;
    bool    _isAuthenticated;
	bool	_isRegistered;
public:
    Client(int fd);
    ~Client();
    
    // Getters
    int getFd() const;
    const std::string& getNickname() const;
    const std::string& getUsername() const;
    const std::string& getRealname() const;
    std::string& getInputBuffer();
    bool    getIsAuthenticated() const;
	bool	getIsRegistered() const;

    // Setters
    void    setNickname(const std::string& nickname);
    void    setUsername(const std::string& username);
    void    setRealname(const std::string& realname);
    void    setIsAuthenticated(bool state);
	void	setIsRegistered(bool state);

    // Public methods
    void    appendBuffer(const std::string& append);
    void    clearBuffer();
};




#endif
