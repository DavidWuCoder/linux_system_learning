.PHONY:all
all:client_netcal ServerNetcald

ServerNetcald:main.cc
	g++ -o $@ $^ -std=c++17 -lpthread -ljsoncpp
client_netcal:TcpClient.cc
	g++ -o $@ $^ -std=c++17 -lpthread -ljsoncpp

.PHONY:
output:
	@mkdir output
	@mkdir -p output/bin
	@mkdir -p output/conf
	@mkdir -p output/log
	@cp ServerNetcald output/bin
	@cp client_netcal output/bin
	@cp test.conf output/conf
	@tar czf output.tgz output 

.PHONY:
clean:
	rm -rf ServerNetcald client_netcal ouput output.tgz