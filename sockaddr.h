#include <string>
#include <sys/socket.h>

class SockAddr
{
public:
	SockAddr();
	SockAddr(struct sockaddr_storage &from, socklen_t saLen);

	std::string getIP();
	int getFamily();
	int getPort();
    
	struct sockaddr *saPtr();
	socklen_t *saLenPtr();

private:
	struct sockaddr_storage sa;
	socklen_t saLen;
};
