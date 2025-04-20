.PHONY:all
all:client_netcal server_netcal

server_netcal:main.cc
	g++ -o $@ $^ -std=c++17 -lpthread -ljsoncpp
client_netcal:TcpClient.cc
	g++ -o $@ $^ -std=c++17 -lpthread -ljsoncpp
.PHONY:
clean:
	rm -rf server_netcal client_netcal

