.PHONY:all
all: udpclient udpserver

udpclient:UdpClient.cc
	g++ -o $@ $^ -std=c++17 -lpthread -static
udpserver:UdpServer.cc
	g++ -o $@ $^ -std=c++17 -lpthread
.PHONY:
clean:
	rm -rf udpclient udpserver

