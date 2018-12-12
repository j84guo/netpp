#include "utils.h"

#include <cstdio>
#include <cstring>

using namespace std;

NetError::NetError(const string &msg):
	runtime_error(msg)
{ }

NetError::NetError(int err):
	runtime_error(strError(err))
{ }

NetError::NetError(const string &prefix, int err):
	runtime_error(prefix + ": " + strError(err))
{ }

string strError(int err)
{
	char buf[128] = {0}, *ptr = nullptr;
#if defined(__GLIBC__) && defined(_GNU_SOURCE)
	ptr = strerror_r(err, buf, sizeof(buf) - 1);
#else
	strerror_r(err, buf, sizeof(buf) - 1);
	ptr = buf;
#endif
	return ptr;
}

string gaiStrError(int err)
{
    return gai_strerror(err);
}


struct addrinfo addrInfoHints(int family, int type, int protocol, int flags)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = family;
	hints.ai_socktype = type;
	hints.ai_protocol = protocol;
	hints.ai_flags = flags;
	return hints;
}

pair<vector<struct addrinfo>, int> getAddrInfo(const string &host,
		const string &port, int family, int type, int protocol, int flags)
{
	struct addrinfo hints = addrInfoHints(family, type, protocol, flags);
	vector<struct addrinfo> infoVec;

	struct addrinfo* info;
	int err = getaddrinfo(host == "" ? nullptr : host.c_str(),
			port == "" ? nullptr : port.c_str(), &hints, &info);
	if (err)
		return make_pair(infoVec, err);
	for (struct addrinfo *ptr = info; ptr; ptr = ptr->ai_next)
		infoVec.push_back(*ptr);

	freeaddrinfo(info);
	return make_pair(infoVec, 0);
}
