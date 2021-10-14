'use strict';

const { Server } = require('bindings')('server'); // bindings устанавливаются

const server = new Server("D:/Develop/nodejs/vs2019/messenger_with_debug/build/Debug/config.json");
server.Initialize();

server.Create();

server.StartListen();

server.HandleClients();