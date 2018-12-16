#ifndef TCPLISTENER_H
#define TCPLISTENER_H

#include "tcpconn.h"

#include <string>
#include <vector>

namespace net 
{

class TCPListener
{
 public:
	TCPListener(const std::string & ip, const std::string & port,
			bool reBind = true);
	~TCPListener();

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
