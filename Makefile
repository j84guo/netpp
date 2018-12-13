# Simple installation for Unix-like systems (this can be cleaned up).
install:
	./scripts/install_lib.sh	
	
# Sketchy AF
uninstall:
	./scripts/uninstall_lib.sh	

demo.out:
	g++ -o $@ -Wall -std=c++17 -I src/ examples/demo.cpp src/*.cpp -lpthread

clean:
	rm -rf *.o *.out

.PHONY: clean install demo.out
