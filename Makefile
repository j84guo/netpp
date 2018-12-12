demo.out:
	g++ -o $@ -Wall -std=c++17 demo.cpp tcpconn.cpp sockaddr.cpp utils.cpp

clean:
	rm -rf *.o *.out
