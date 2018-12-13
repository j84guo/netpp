### Network Programming for Humans
We're well into the 21st century, stop fiddling with `struct sockaddr_storage`
and `getaddrinfo`! This a simple socket library inspired by Golang's `net`
package.

![Alt text](img/furries.jpg?raw=true "Lol")

### Installation
```make install```

### Usage
Netpp is distributed as a shared library with headers, so you'll have to run
the ```make install``` target (above) in order to compile code using this
libary.

Usage in your code is extremely simple. You can access the whole lirbary by
including ```netpp/net.h```. All functions/types exist withint the ```net```
namespace. The snippet bellow shows usage of ```class TCPConn```.

```js
#include <netpp/net.h>

#include <iostream>
#include <vector>

using net::TCPConn;
using std::cout;
using std::string;
using std::vector;

int main()
{
	string req = "GET / HTTP/1.1\r\nConnection: close\r\n\r\n";

	TCPConn conn("google.ca", "80");
	conn.sendAll(req.c_str(), req.size());

	vector<char> buf;
	long num = conn.recvAll(buf);
	cout << string(buf.begin(), buf.begin() + num) << '\n';
}
```

### Demo (see examples/demo.cpp)
```
# Build demo
make demo.out

# Mode 1: HTTP GET request
./demo.out google.ca 80
./demo.out <domainName> <portNumber>

# Mode 2: Simple Multi-threaded TCP Server
./demo.out

# (From a seperate shell tab)
nc localhost 8000

# Send stuff...
# And then press ctrl-D to end netcat. The server will print the data you sent
# once the TCP connection is closed.
```
