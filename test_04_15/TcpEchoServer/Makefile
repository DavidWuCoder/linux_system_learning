.PHONY:all
all: tcpclient tcpserver

tcpclient:TcpClient.cc
	g++ -o $@ $^ -std=c++17 -lpthread -static
tcpserver:TcpServer.cc
	g++ -o $@ $^ -std=c++17 -lpthread
.PHONY:
clean:
	rm -rf tcpclient tcpserver

