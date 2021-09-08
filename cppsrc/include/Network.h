#pragma once

#include "Socket.h"
#include <napi.h>
#include <iostream>
#include <string>

class Network : public Napi::ObjectWrap<Network> {
public:
	//Network(const Napi::CallbackInfo& info);
	Napi::Boolean Initialize(const Napi::CallbackInfo& info);	// bool
	Napi::Value Shutdown(const Napi::CallbackInfo& info);		// void
	Napi::Boolean Create(const Napi::CallbackInfo& info);		// number
	Napi::Boolean Close(const Napi::CallbackInfo& info);
	Socket GetMainSocket();

protected:
	Socket main_socket;
};