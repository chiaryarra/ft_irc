NAME = ircserv

CPP = c++

FLAGS = -Wall -Werror -Wextra -std=c++98
	   
SRCS = src/general/main.cpp src/server/Server.cpp src/server/ServerNetworking.cpp src/server/ServerMessages.cpp src/client/Client.cpp src/server/Channel.cpp \
src/commands/PASS.cpp src/commands/NICK.cpp src/commands/USER.cpp src/commands/JOIN.cpp \
src/commands/MODE.cpp src/commands/INVITE.cpp src/commands/TOPIC.cpp src/commands/KICK.cpp \
src/commands/PRIVMSG.cpp src/commands/PART.cpp src/utils/Utils.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CPP) $(FLAGS) $(OBJS) -o $(NAME)

.cpp.o:
	$(CPP) $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean $(NAME)
