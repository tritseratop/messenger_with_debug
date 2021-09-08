#pragma once
#include "IncludeMe.h"
#include <iostream>
#include <list>
#include <queue>
#include <string>

#include <napi.h>

class Client : public Napi::ObjectWrap<Client> {
public:
	Client(const Napi::CallbackInfo& info);
	static Napi::Object Init(Napi::Env, Napi::Object exports);
	static Napi::Value CreateNewItem(const Napi::CallbackInfo& info);

	Napi::Value Initialize(const Napi::CallbackInfo& info);	// bool
	Napi::Value Shutdown(const Napi::CallbackInfo& info);	// void
	Napi::Value Create(const Napi::CallbackInfo& info);		// number
	Napi::Value Close(const Napi::CallbackInfo& info);

	Result Connect(Endpoint endpoint);
	Napi::Value Connect(const Napi::CallbackInfo& info);
	Napi::Value StartChating(const Napi::CallbackInfo& info);
private:
	Socket main_socket;
	std::list<Socket> client_sockets;
	std::queue<Socket> waiting_clients; // TODO очередь ожидания
	size_t client_count = 0;
};
