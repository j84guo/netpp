### Network Programming for Humans
We're well into the 21st century, stop fiddling with `struct sockaddr_storage`
and `getaddrinfo`! This a simple socket library inspired by Golang's `net`
package.

### Build
```make```

### Demo
```
# Mode 1 HTTP GET request
./demo.out google.ca 80

# Mode 2: Simple Multi-threaded TCP Server
./demo.out

# (From a seperate shell tab)
nc localhost 8000

# Send stuff...
# And then press ctrl-D to end netcat. The server will print the data you sent
# once the TCP connection is closed.
```
