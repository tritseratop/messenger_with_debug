#pragma once

const int MAX_CLIENT_COUNT = 3;
const int MAX_MESSAGE_BUF_COUNT = 5;
const int PORT = 1111;
const char* const IP = "127.0.0.1";

struct Configure {
	Configure(int N = 3, int M = 5, int port = 1111, char* ip = "127.0.0.1")
		: MAX_CLIENT_COUNT(N)
		, MAX_MESSAGE_BUF_COUNT(M)
		, PORT(port)
		, IP(ip)
	{}
	const int MAX_CLIENT_COUNT;
	const int MAX_MESSAGE_BUF_COUNT;
	const int PORT;
	const char* const IP;
};