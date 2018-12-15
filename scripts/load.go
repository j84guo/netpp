package main

import (
	"io"
	"os"
	"fmt"
	"net"
	"time"
	"sync"
	"strconv"
)

var wg sync.WaitGroup

func connectAndLoad(host string) {
	defer wg.Done()
	conn, e := net.Dial("tcp", host)
	if e != nil {
		fmt.Fprintf(os.Stderr, e.Error() + "\n")
		return
	}
	defer conn.Close()

	buf := make([]byte, 4096)
	for i := 0; i < 500000; i++ {
		_, e := conn.Write(buf)
		if e != nil {
			if e == io.EOF {
				break
			}
			panic(e)
		}

		_, e = conn.Read(buf)
		if e != nil {
			if e == io.EOF {
				break
			}
			panic(e)
		}
	}
	fmt.Println("Connection closed:", conn.RemoteAddr())
}

func main() {
	if len(os.Args) != 2 {
		fmt.Fprintf(os.Stderr, "Usage: %s <numClients>\n", os.Args[0])
		os.Exit(1)
	}

	num, e := strconv.Atoi(os.Args[1])
	if e != nil {
		panic(e)
	}

	for i := 0; i<num; i+=1 {
			wg.Add(1)
		go connectAndLoad("localhost:8000")
		time.Sleep(100 * time.Millisecond)
	}
	wg.Wait()
}
