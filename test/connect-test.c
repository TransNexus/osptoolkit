#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>

int main(int argc, char *argv[]) {

	const char* ip = argv[1];

	struct sockaddr_in sin;

	sin.sin_family      = AF_INET;
	sin.sin_addr.s_addr = inet_addr(ip);
	sin.sin_port        = 5045;

	int e;
	int s = socket(AF_INET, SOCK_STREAM, 0);

	// follows nonblock
	//e = fcntl(s, F_SETFL, O_NONBLOCK);

	// still blocks untill system wide connect timeout
	e = fcntl(s, F_SETFD, O_NONBLOCK);

	e = connect(s, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));

	fprintf(stderr,"IP = %s e = %d errno = %s\n",ip,e,strerror(errno));

	int timeout = 200;
	struct pollfd fd;
	fd.fd = s;
	fd.events = POLLOUT;
	fd.revents = 0;

	int fdready = poll(&fd, 1, timeout);

	fprintf(stderr,"fdready = %d\n",fdready);
}
