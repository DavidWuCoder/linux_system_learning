.PHONY:all
all: client server

client:UdpClient.cc
	g++ -o $@ $^ -std=c++17
server:UdpServer.cc
	g++ -o $@ $^ -std=c++17
.PHONY:
clean:
	rm -rf client server

