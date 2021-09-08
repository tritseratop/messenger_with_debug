#pragma once
#include <string>
#include <vector>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

class Endpoint {
public:
	Endpoint(const char* ip, unsigned short port);
	Endpoint(sockaddr* addr);
	std::string GetHostname();
	std::string GetIPString();
	std::vector<uint8_t> GetIPBytes();
	unsigned short GetPort();
	sockaddr_in GetSockaddrIPv4();
private:
	std::string hostname = "";
	std::string ip_string = "";
	std::vector<uint8_t> ip_bytes;
	unsigned short port = 0;
};