#include "Socket.h"
#include <iostream>

Socket::Socket(SOCKET handle)
	: handle(handle) {
}

Result Socket::Create() {
	if (handle != INVALID_SOCKET) {
		return Result::Error;
	}
	handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (handle == INVALID_SOCKET) {
		int error = WSAGetLastError();
		return Result::Error;
	}
	return Result::Success;
}

Result Socket::Close() {
	if (handle == INVALID_SOCKET) {
		return Result::Error;
	}
	int result = closesocket(handle);
	if (result != 0) {
		int error = WSAGetLastError();
		return Result::Error;
	}
	handle = INVALID_SOCKET;
	return Result::Success;
}

Result Socket::Bind(Endpoint endpoint) {
	sockaddr_in addr = endpoint.GetSockaddrIPv4();
	int result = bind(handle, (sockaddr*)&addr, sizeof(sockaddr_in));
	if (result != 0) {
		int error = WSAGetLastError();
		return Result::Error;
	}
	return Result::Success;
}

Result Socket::Listen(Endpoint endpoint, int backlog) {
	if (Bind(endpoint) == Result::Error) {
		return Result::Error;
	}
	int result = listen(handle, backlog);
	if (result != 0) {
		int error = WSAGetLastError();
		return Result::Error;
	}
	return Result::Success;
}

Result Socket::Listen(std::string ip, int port) {
	SOCKADDR_IN addr;								// Хранит информацию об адресе сокета
	inet_pton(AF_INET, ip.c_str(), &addr.sin_addr.s_addr); //IP
	addr.sin_port = htons(port);					// Port
	addr.sin_family = AF_INET;
	return Listen( Endpoint((sockaddr*)&addr) );
}

Result Socket::Accept(Socket& outSocket) {
	sockaddr_in addr = {};
	int length = sizeof(sockaddr_in);
	SOCKET acceptedConnectionHandle = accept(handle, (sockaddr*)&addr, &length);
	if (acceptedConnectionHandle == INVALID_SOCKET) {
		int error = WSAGetLastError();
		return Result::Error;
	}
	Endpoint newConnectionEndpoint((sockaddr*)&addr);
	std::cout << "New connection accepted!" << std::endl;
	outSocket = Socket(acceptedConnectionHandle);
	return Result::Success;
}

Result Socket::Connect(Endpoint endpoint) {
	sockaddr_in addr = endpoint.GetSockaddrIPv4();
	int result = connect(handle, (sockaddr*)&addr, sizeof(sockaddr_in));
	if (result != 0) {
		int error = WSAGetLastError();
		return Result::Error;
	}
	return Result::Success;
}

Result Socket::Send(const void* data, int numberOfBytes, int& bytesSent) {
	bytesSent = send(handle, (const char*)data, numberOfBytes, NULL);
	if (bytesSent == SOCKET_ERROR) {
		int error = WSAGetLastError();
		return Result::Error;
	}
	return Result::Success;
}

Result Socket::Recv(void* data, int numberOfBytes, int& bytesSent) {
	bytesSent = recv(handle, (char*)data, numberOfBytes, NULL);
	if (bytesSent == SOCKET_ERROR) {
		int error = WSAGetLastError();
		return Result::Error;
	}
	return Result::Success;
}

Result Socket::Send(std::string& message) {
	int bytesSent = 0;
	const char* msg = message.c_str();
	int size = message.size();
	// Отправляем размер сообщения
	Result res = Send(&size, sizeof(int), bytesSent);
	if (bytesSent == SOCKET_ERROR) {
		int error = WSAGetLastError();
		return Result::Error;
	}
	// Отправляем сообщение
	res = Send(message.c_str(), size, bytesSent);
	if (bytesSent == SOCKET_ERROR) {
		int error = WSAGetLastError();
		return Result::Error;
	}
}

Result Socket::Recv(std::string& message) {
	int bytesRecieved = 0;
	int size;
	// Принимаем размер
	Recv(&size, sizeof(int), bytesRecieved);
	if (bytesRecieved == SOCKET_ERROR) {
		int error = WSAGetLastError();
		return Result::Error;
	}
	// Подготовливаем хранилище сообщения
	message.resize(size);
	//message[size] = '\0';

	// Принимаем сообщение
	Recv(&message[0], size, bytesRecieved);
	if (bytesRecieved == SOCKET_ERROR) {
		int error = WSAGetLastError();
		return Result::Error;
	}
	return Result::Success;
}

SOCKET Socket::GetSocketHandle() {
	return handle;
}