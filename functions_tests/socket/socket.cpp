#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
int	main()
{
	int	fd;

	fd = socket(AF_UNIX, SOCK_STREAM, 0);
}
