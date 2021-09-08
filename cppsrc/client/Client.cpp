#include "Client.h"
#include <thread>
#include <future>
#include <iostream>

using CallInput = Napi::Value (*)(const Napi::CallbackInfo& info);

// Data structure representing our thread-safe function context.
struct TsfnContext {
	TsfnContext(std::string m) :  msg(m) { }

	// Native thread
	std::thread nativeThread;

	// Some data to pass around
	std::string msg;

	Napi::ThreadSafeFunction tsfn;
};

void FinalizerCallback(Napi::Env env,
	void* finalizeData,
	TsfnContext* context) {
	// Join the thread
	context->nativeThread.join();
	delete context;
}

Client::Client(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Client>(info) {}

Napi::Value Client::Initialize(const Napi::CallbackInfo& info) {
	WSADATA wsadata;
	int result = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (result != 0) {
		std::cerr << "Failed to start up the winsock API." << std::endl;
		return Napi::Boolean::New(info.Env(), false);
	}
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		std::cerr << "Could not find a usable version of the winsock API dll." << std::endl;
		return Napi::Boolean::New(info.Env(), false);
	}
	std::cout << "Winsock API successfully initialized!" << std::endl;
	return Napi::Boolean::New(info.Env(), true);
}

Napi::Value Client::Shutdown(const Napi::CallbackInfo& info) { // TODO ��� ������ � void?
	WSACleanup();
	return Napi::Boolean::New(info.Env(), true);
}

Napi::Value Client::Create(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	return Napi::Boolean::New(env, main_socket.Create());
}

Napi::Value Client::Close(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	return Napi::Boolean::New(env, main_socket.Close());
}

Result Client::Connect(Endpoint endpoint) {
	return main_socket.Connect(endpoint);
}

Napi::Value Client::Connect(const Napi::CallbackInfo& info) {
	return Napi::Boolean::New(info.Env(), main_socket.Connect(Endpoint(IP, PORT)));
}

// ������
Napi::Value Client::StartChating(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	TsfnContext* context = new TsfnContext("");
	context->tsfn = Napi::ThreadSafeFunction::New(
		env,							// Environment
		info[1].As<Napi::Function>(),	// JS function from caller
		"TSFN",							// Resource name
		0,								// Max queue size (0 = unlimited).
		1,								// Initial thread count
		context,						// Context,
		FinalizerCallback,				// Finalizer
		(void*)nullptr					// Finalizer data
	);
	std::cout << "Function connected" << std::endl;
	context->nativeThread = std::thread(
		[this, &context]() {
			auto callback = [](Napi::Env env, Napi::Function jsCallback, std::string* message) {
				// Transform native data into JS data, passing it to the provided
				// `jsCallback` -- the TSFN's JavaScript function.
				jsCallback.Call({ Napi::String::New(env, *message) });
			};
			std::string message;
			while (true) {
				if (main_socket.Recv(message) == Result::Error) {
					int error = WSAGetLastError();
					return;
				}
				// TODO send to js
				std::cout << message << std::endl;
				context->tsfn.BlockingCall(&message, callback);
			}
		}
	);
	Napi::Function input = info[0].As<Napi::Function>();
	std::string send_msg;
	while (true) {
		// TODO recv from js
		send_msg = input.Call(info.Env().Global(), {}).As<Napi::String>();
		//std::getline(std::cin, send_msg);
		if (main_socket.Send(send_msg) == Result::Error) {
			int error = WSAGetLastError();
			return Napi::Boolean::New(info.Env(), false);
		}
	}
}

Napi::Value Client::CreateNewItem(const Napi::CallbackInfo& info) {
	Napi::FunctionReference* constructor =
		info.Env().GetInstanceData<Napi::FunctionReference>();
	return constructor->New({ });
}

Napi::Object Client::Init(Napi::Env env, Napi::Object exports) {
	Napi::Function func = DefineClass(env, "Client", {
		InstanceMethod<&Client::Initialize>("Initialize"),
		InstanceMethod<&Client::Shutdown>("Shutdown"),
		InstanceMethod<&Client::Create>("Create"),
		InstanceMethod<&Client::Close>("Close"),
		InstanceMethod<&Client::Connect>("Connect"),
		InstanceMethod<&Client::StartChating>("StartChating"),
		});
	Napi::FunctionReference* constructor = new Napi::FunctionReference();

	*constructor = Napi::Persistent(func);
	exports.Set("Client", func);

	env.SetInstanceData<Napi::FunctionReference>(constructor);
	return exports;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
	Client::Init(env, exports);
	return exports;
}

NODE_API_MODULE(client, Init)