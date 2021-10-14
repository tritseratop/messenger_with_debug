#pragma once

const int MAX_CLIENT_COUNT = 3;
const int MAX_MESSAGE_BUF_COUNT = 5;
const int PORT = 1111;
const char* const IP = "127.0.0.1";

class Configure {
public:
	Configure(int N = 3, int M = 5, int port = 1111, std::string ip = "127.0.0.1")
		: MAX_CLIENT_COUNT(N)
		, MAX_MESSAGE_BUF_COUNT(M)
		, PORT(port)
		, IP(ip)
	{}

	/*Configure& operator= (const Configure& config) {
		if (this == &config) {
			return *this;
		}
		MAX_CLIENT_COUNT = config.getMaxClientCount();
		MAX_MESSAGE_BUF_COUNT = config.getMaxMessageBufCount();
		PORT = config.getPort();
		IP = config.getIp();
		return *this;
	}*/

	int getMaxClientCount() const {
		return MAX_CLIENT_COUNT;
	}
	int getMaxMessageBufCount() const {
		return MAX_MESSAGE_BUF_COUNT;
	}
	int getPort() const {
		return PORT;
	}
	std::string getIp() const {
		return IP;
	}
private:
	int MAX_CLIENT_COUNT;
	int MAX_MESSAGE_BUF_COUNT;
	int PORT;
	std::string IP;
};