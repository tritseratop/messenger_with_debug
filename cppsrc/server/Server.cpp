#include "Server.h"
#include <iostream>
#include "Utility.h"

using LOG = logger::FileLogger::e_logType;

void ServerClientHandler(SOCKET client) {
}

Napi::Value Server::Initialize(const Napi::CallbackInfo& info) {
    WSADATA wsadata;
    int result = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (result != 0) {
        log << LOG::LOG_ERROR << "Failed to start up the winsock API";
        return Napi::Boolean::New(info.Env(), false);
    }
    if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
        log << LOG::LOG_ERROR << "Could not find a usable version of the winsock API dll";
        return Napi::Boolean::New(info.Env(), false);
    }
    log << LOG::LOG_INFO << "Winsock API successfully initialized";
    return Napi::Boolean::New(info.Env(), true);
}

Napi::Value Server::Shutdown(const Napi::CallbackInfo& info) { // TODO ��� ������ � void?
    WSACleanup();
    return Napi::Boolean::New(info.Env(), true);
}

Napi::Value Server::Create(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, main_socket.Create());
}

Napi::Value Server::Close(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, main_socket.Close());
}

std::string createMessageFromQueue(const std::deque<std::string>& deq) {
    std::string res;
    for (const auto& d : deq) {
        res += d + "\n";
    }
    res.pop_back();
    return res;
}

Result Server::AddClient(Socket& client) { // TODO ����������� � ������������ � �������
    if (client_sockets.size() < MAX_CLIENT_COUNT) {
        client_it[client.GetSocketHandle()] = client_sockets.insert(client_sockets.end(), client);
        FD_SET(client.GetSocketHandle(), &master);
        std::string welcomeMsg = "Welcome to the Awesome Chat Server!\r"; // TODO receive from js
        client.Send(welcomeMsg);
        if (!message_history.empty()) {
            client.Send(createMessageFromQueue(message_history));
        }
        return Result::Success;
    }
    else {
        waiting_clients.push(client);
        log << LOG::LOG_INFO << "Client #" + std::to_string(client.GetSocketHandle()) + " is added to waiting";
        std::string msg = "The maximum number of messenger clients was reached. Please wait...";
        client.Send(msg);
        return Result::Success;
    }
}

Result Server::StartListen(Endpoint endpoint) {
	return main_socket.Listen(endpoint);
}

Napi::Value Server::StartListen(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), main_socket.Listen(config.getIp(), config.getPort()));
}

Napi::Value Server::HandleClients(const Napi::CallbackInfo& info) {
    //fd_set master;
    FD_ZERO(&master);
    FD_SET(main_socket.GetSocketHandle(), &master);

    while (true) {
        fd_set copy = master;
        int socket_count = select(0, &copy, nullptr, nullptr, nullptr);
        for (int i = 0; i < socket_count; ++i) {
            Socket sock(copy.fd_array[i]);
            if (sock.GetSocketHandle() == main_socket.GetSocketHandle()) {
                Socket client;
                Result res = main_socket.Accept(client);
                if (res == Result::Error) {
                    log << LOG::LOG_ERROR << "Failed to connect client #" + std::to_string(client.GetSocketHandle());
                    int error = WSAGetLastError();
                    return Napi::Boolean::New(info.Env(), false);
                }
                AddClient(client);
                log << LOG::LOG_INFO << "Client #" + std::to_string(client.GetSocketHandle()) + " is connected";
            }
            else {
                std::string message;
                if (sock.Recv(message) != Result::Success) {
                    DeleteSocket(sock);
                }
                else {
                    SendToAll(message, sock);
                }
            }
        }
    }
    return Napi::Boolean::New(info.Env(), true); // TODO ��������� �������� ������������ �����
}

Result Server::SendToAll(std::string msg, const Socket& from) {
    for (auto& s : client_sockets) {
        if (from.GetSocketHandle() != s.GetSocketHandle()) {
            std::string msg_to_send = "Client #" + std::to_string(from.GetSocketHandle()) + " " + msg;
            log << LOG::LOG_MESSAGE << msg_to_send;
            if (message_history.size() == MAX_MESSAGE_BUF_COUNT) {
                message_history.pop_front();
            }
            message_history.push_back(msg_to_send);
            if (s.Send(msg_to_send) != Result::Success) { // TODO use move
                DeleteSocket(s);
            }
        }
    }
    return Result::Success;
}

void Server::DeleteSocket(Socket& s) {
    FD_CLR(s.GetSocketHandle(), &master);
    int handle = s.GetSocketHandle();
    s.Close();
    client_sockets.erase(client_it.at(handle)); //� ����� �������?
    client_it.erase(handle);
    log << LOG::LOG_INFO << "Client #" + std::to_string(handle) + " is disconnected";
    if (!waiting_clients.empty()) {
        AddClient(waiting_clients.front());
        log << LOG::LOG_INFO << "Client #" + std::to_string(waiting_clients.front().GetSocketHandle()) + " is added to chat";
        waiting_clients.pop();
    }
}

Server::Server(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<Server>(info)
    , log("Messenger Logger")
{
    SetConfig(info[0].As<Napi::String>());
}



void Server::SetConfig(const std::string& path) {
    std::string json;
    if (ReadTextFile(path, json) == Result::Success) {
        config = ParseJsonToConfig(json);
    }
}

Napi::Object Server::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "Server", {
        InstanceMethod<&Server::Initialize>("Initialize"),
        InstanceMethod<&Server::Shutdown>("Shutdown"),
        InstanceMethod<&Server::Create>("Create"),
        InstanceMethod<&Server::Close>("Close"),
        InstanceMethod<&Server::StartListen>("StartListen"),
        InstanceMethod<&Server::HandleClients>("HandleClients"),
        StaticMethod<&Server::CreateNewItem>("CreateNewItem"),
        });
    Napi::FunctionReference* constructor = new Napi::FunctionReference();

    *constructor = Napi::Persistent(func);
    exports.Set("Server", func);

    env.SetInstanceData<Napi::FunctionReference>(constructor);
    return exports;
}

Napi::Value Server::CreateNewItem(const Napi::CallbackInfo& info) {
    Napi::FunctionReference* constructor =
        info.Env().GetInstanceData<Napi::FunctionReference>();
    return constructor->New({ });
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Server::Init(env, exports);
    return exports;
}

NODE_API_MODULE(server, Init)