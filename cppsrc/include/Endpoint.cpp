#include "Endpoint.h"
#include <assert.h>

Endpoint::Endpoint(const char* ip, unsigned short port) {
	this->port = port;
	in_addr addr;
	int result = inet_pton(AF_INET, ip, &addr);

	if (result == 1) {
		if (addr.S_un.S_addr != INADDR_NONE) {
			ip_string = ip;
			hostname = ip;
			ip_bytes.resize(sizeof(ULONG));
			memcpy(&ip_bytes[0], &addr.S_un.S_addr, sizeof(ULONG));
			return;
		}
	}
}

Endpoint::Endpoint(sockaddr* addr) {
	assert(addr->sa_family == AF_INET);
	sockaddr_in* addrv4 = reinterpret_cast<sockaddr_in*>(addr);
	port = ntohs(addrv4->sin_port);
	ip_bytes.resize(sizeof(ULONG));
	memcpy(&ip_bytes[0], &addrv4->sin_addr, sizeof(ULONG));

	ip_string.resize(16);
	inet_ntop(AF_INET, &addrv4->sin_addr, &ip_string[0], 16);
	hostname = ip_string;
}

std::string Endpoint::GetHostname() {
	return hostname;
}
std::string Endpoint::GetIPString() {
	return ip_string;
}
std::vector<uint8_t> Endpoint::GetIPBytes() {
	return ip_bytes;
}
unsigned short Endpoint::GetPort() {
	return port;
}
sockaddr_in Endpoint::GetSockaddrIPv4() {
	sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	memcpy(&addr.sin_addr, &ip_bytes[0], sizeof(ULONG));
	addr.sin_port = htons(port); // host-to-network преобразует байты в network формат big-endian
	return addr;
}

