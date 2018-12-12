#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <stdexcept>

#include <netdb.h>

/* A runtime_error which can search for a message given an OS error code. Thrown
   by network operations, e.g. TcpConn's constructor. */
class NetError : public std::runtime_error
{
public:
	NetError(const std::string &msg);
	NetError(int err);
	NetError(const std::string &prefix, int err);
};

/* Returns a string for the specifed OS error code using strerror_r(). */
std::string strError(int err);

/* Returns a string for the specifed gai error code using gai_strerror(). */
std::string gaiStrError(int err);

/* Returns a structure which can be passed as a hint to getaddrinfo(), using
   the specified family, type, protocol and flags.

   E.g.
   auto clientHints = addrInfoHints(AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0);
   auto serverHints = addrInfoHints(AF_UNSPEC, 0, 0, AI_PASSIVE); */
struct addrinfo addrInfoHints(int family, int type, int protocol, int flags);

/* Returns a vector of addrinfo structures for the specified host; pair::second
   holds a getaddrinfo() error on failure, for which a messaeg can be retrieved
   via gaiStrError(). */
std::pair<std::vector<struct addrinfo>, int> getAddrInfo(
        const std::string &host, const std::string &port, int family, int type,
        int protocol, int flags = 0);

#endif
