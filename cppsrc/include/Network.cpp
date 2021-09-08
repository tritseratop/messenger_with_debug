#include "Network.h"

//Network::Network(const Napi::CallbackInfo& info) {
//	Napi::ObjectWrap<Example>(info) {
//	Napi::Env env = info.Env();
//}

Napi::Boolean Network::Initialize(const Napi::CallbackInfo& info) {
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

Napi::Value Network::Shutdown(const Napi::CallbackInfo& info) { // TODO что делать с void?
	WSACleanup();
	return Napi::Boolean::New(info.Env(), true);
}

Napi::Boolean Network::Create(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	return Napi::Boolean::New(env, main_socket.Create());
}

Napi::Boolean Network::Close(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	return Napi::Boolean::New(env, main_socket.Close());
}

Socket Network::GetMainSocket() {
	return main_socket;
}