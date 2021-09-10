#include "Client.h"
#include <thread>
#include <future>
#include <iostream>

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

// ЖИЕСТЬ
Napi::Value Client::StartChating(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	if (info.Length() < 2) {
		std::cout << "Expected two arguments" << std::endl;
		//return  Napi::Boolean::New(info.Env(), false);
	}
	else if (!info[0].IsFunction()) {
		std::cout << "Expected first arg to be function" << std::endl;
		//return  Napi::Boolean::New(info.Env(), false);
	}
	else if (!info[1].IsFunction()) {
		std::cout << "Expected second arg to be function" << std::endl;
		//return  Napi::Boolean::New(info.Env(), false);
	}
	std::thread native_thread;
	Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(
		env,							// Environment
		info[0].As<Napi::Function>(),	// JS function from caller
		"TSFN",							// Resource name
		0,								// Max queue size (0 = unlimited).
		1,								// Initial thread count
		[&native_thread](Napi::Env) {        // Finalizer used to clean threads up
			native_thread.join();
		}
	);
	std::cout << "Function connected" << std::endl;
	native_thread = std::thread(
		[this, &tsfn]() {
			auto callback = [](Napi::Env env, Napi::Function jsCallback, std::string* message) {
				// Transform native data into JS data, passing it to the provided
				// `jsCallback` -- the TSFN's JavaScript function.
				jsCallback.Call({ Napi::String::New(env, *message) });
				delete message;
			};
			while (true) {
				std::string* message = new std::string();
				if (main_socket.Recv(*message) == Result::Error) {
					int error = WSAGetLastError();
					return;
				}
				// TODO send to js
				
				std::cout << *message << std::endl;

				
				// dont work
				napi_status status = tsfn.NonBlockingCall(message, callback);
				if (status != napi_ok) {
					//Napi::Error::Fatal(
					//	"ThreadEntry",
					//	"Napi::ThreadSafeNapi::Function.BlockingCall() failed");
					std::cout << "Napi::ThreadSafeNapi::Function.BlockingCall() failed" << std::endl;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
			}
			tsfn.Release();
		}
	);
	native_thread.detach();
	Napi::Function input = info[1].As<Napi::Function>();
	std::string send_msg;
	while (true) {
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