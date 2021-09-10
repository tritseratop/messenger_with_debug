#pragma once
#include "IncludeMe.h"
#include <iostream>
#include <list>
#include <queue>
#include <string>

#include <napi.h>

class EchoWorker : public Napi::AsyncProgressQueueWorker<std::string> {
public:
	EchoWorker(Napi::Function & okCallback, Napi::Function& errorCallback, Napi::Function& progressCallback, std::string& echo)
	: AsyncProgressQueueWorker(okCallback), echo(echo){
		this->errorCallback.Reset(errorCallback, 1);
		this->progressCallback.Reset(progressCallback, 1);
	}
	~EchoWorker() {}

	void Execute(const ExecutionProgress& progress) {
		// Need to simulate cpu heavy task to demonstrate that
		// every call to Send() will trigger an OnProgress function call
		for (uint32_t i = 0; i < 100; ++i) {
			std::string str = "hello";
			progress.Send(&str, 1);
		}
	}
	void OnProgress(const std::string* data, size_t /* count */) {
		Napi::HandleScope scope(Env());

		if (!this->progressCallback.IsEmpty()) {
			// Call our onProgressCallback in javascript with each integer from 0 to 99 (inclusive)
			// as this function is triggered from the above Send() calls
			this->progressCallback.Call(Receiver().Value(), { Napi::String::New(Env(), *data) });
		}
	}
	void OnOK() {
		Napi::HandleScope scope(Env());
		// Call our onOkCallback in javascript with the data we were given originally
		Callback().Call({ Napi::String::New(Env(), echo) });
	}

	void OnError(const Napi::Error& e) {
		Napi::HandleScope scope(Env());
		// We call our callback provided in the constructor with 2 parameters
		if (!this->errorCallback.IsEmpty()) {
			// Call our onErrorCallback in javascript with the error message
			this->errorCallback.Call(Receiver().Value(), { Napi::String::New(Env(), e.Message()) });
		}
	}

private:
	std::string echo;
	Napi::FunctionReference progressCallback;
	Napi::FunctionReference errorCallback;
};

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
	EchoWorker* worker;
};
