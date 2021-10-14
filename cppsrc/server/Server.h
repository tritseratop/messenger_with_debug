#pragma once
#include "IncludeMe.h"
#include "logger/Logger.h"
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <string>

#include <napi.h>

class Server : public Napi::ObjectWrap<Server> {
public:
	Server(const Napi::CallbackInfo& info);
	static Napi::Object Init(Napi::Env, Napi::Object exports);
	static Napi::Value CreateNewItem(const Napi::CallbackInfo& info);

	Napi::Value Initialize(const Napi::CallbackInfo& info);	// bool
	Napi::Value Shutdown(const Napi::CallbackInfo& info);	// void
	Napi::Value Create(const Napi::CallbackInfo& info);		// number
	Napi::Value Close(const Napi::CallbackInfo& info);

	Result StartListen(Endpoint endpoint);
	Napi::Value StartListen(const Napi::CallbackInfo& info);
	Napi::Value HandleClients(const Napi::CallbackInfo& info);
	//friend void ClientHandler(Server& server);
	Result AddClient(Socket& client);
	void SetConfig(const std::string& path);
private:
	std::ofstream file;
	logger::FileLogger log;
	fd_set master;
	Socket main_socket;
	std::list<Socket> client_sockets;
	std::map<int, std::list<Socket>::iterator> client_it;
	std::queue<Socket> waiting_clients;
	std::deque<std::string> message_history;
	Configure config;
	//Socket server_socket;
	size_t client_count = 0;
	Result SendToAll(std::string msg, const Socket& from);
	void DeleteSocket(Socket& s);
	//fd_set master;
};