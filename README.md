# ft_irc

An IRC server implementation in C++98, using poll()-based I/O multiplexing.

## Team
- chiaryarra
- hebaja

## Usage

```sh
make
./ircserv <port> <password>
```

### Build targets
| Target    | Description                    |
|-----------|--------------------------------|
| `all`     | Compile `ircserv`              |
| `clean`   | Remove object files            |
| `fclean`  | Remove object files + binary   |
| `re`      | Full rebuild                   |

Compilation uses `-std=c++98 -Wall -Werror -Wextra`.

## Registration

All three commands are required before registration completes. They can be sent in any order:

- `PASS <password>` — authenticate with server password
- `NICK <nickname>` — set nickname (1–9 chars, alphanumeric + `[]\`^{}\|-`)
- `USER <username> <mode> <unused> <realname>` — set user info (`mode` must be `0`, `unused` must be `*`)

Once all three are received, server sends RPL 001–004 welcome messages.

## Commands

### Channel commands
| Command | Syntax | Description |
|---------|--------|-------------|
| JOIN | `JOIN <channel>` | Join or create a channel |
| PART | `PART <channel> [message]` | Leave a channel |
| KICK | `KICK <channel> <nick> [message]` | Kick user from channel (operator only) |
| INVITE | `INVITE <nick> <channel>` | Invite user to channel |
| TOPIC | `TOPIC <channel> [topic]` | View or set channel topic |
| MODE | `MODE <channel> <modes> [params]` | Set/unset channel modes |
| PRIVMSG | `PRIVMSG <target> <message>` | Send message to channel or user |

### Other commands
| Command | Description |
|---------|-------------|
| QUIT | Disconnect from server |
| PING | Reply with PONG |
| CAP | Silently ignored (client capability negotiation) |

## Channel modes

| Mode | Parameter | Description |
|------|-----------|-------------|
| `+i` | — | Invite-only channel |
| `+t` | — | Topic protected (only operators can change) |
| `+k` | `<key>` | Channel password |
| `+o` | `<nick>` | Grant/revoke operator status |
| `+l` | `<limit>` | Maximum number of users |

Multiple modes can be combined (e.g., `+itk secret`).

## Architecture

- Single-threaded with non-blocking sockets
- `poll()` for I/O multiplexing
- Command dispatch via `std::map<std::string, handler>`
- State: `Server` owns `map<int, Client>` and `map<string, Channel>`
- Channel operators tracked per-channel; first user becomes operator
- Empty channels are automatically removed
