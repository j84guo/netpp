#include "utils.h"
#include "tcpconn.h"

#include <string>
#include <vector>
#include <utility>

#include <cstddef>
#include <cstring>
#include <errno.h>
#include <unistd.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

using namespace std;

class TCPServer
{
public:
    TCPServer(const std::string &ip, const std::string &port);
    ~TCPServer();

private:
    SockAddr localAddr;
};

int main()
{

}
