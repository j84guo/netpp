#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "tcpconn.h"

#include <string>
#include <vector>

namespace net 
{

class TCPServer
{
 public:
	TCPServer(const std::string & ip, const std::string & port,
			bool reBind = true);
	~TCPServer();

	TCPConn accept();
	SockAddr localAddr();

 private:
	bool bindWithFirst(std::vector<struct addrinfo> &infoVec);
	bool listen();
	bool socket(int family, int type, int protocol);

	bool reBind;
	int sockDes;
	SockAddr local;
};

} /* namespace net */

#endif
