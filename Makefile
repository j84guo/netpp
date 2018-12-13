# Simple installation for Unix-like systems (this can be cleaned up).
install:
	./scripts/install_lib.sh	
	
# Sketchy AF
uninstall:
	./scripts/uninstall_lib.sh	

# Also kind of sketchy
demo:
	g++ -o demoClient.out -Wall -std=c++17 -I src/ examples/demoClient.cpp src/*.cpp -lpthread
	g++ -o demoServer.out -Wall -std=c++17 -I src/ examples/demoServer.cpp src/*.cpp -lpthread
	g++ -o echoServer.out -Wall -std=c++17 -I src/ examples/echoServer.cpp src/*.cpp -lpthread

clean:
	rm -rf *.o *.out

.PHONY: clean install demo
