'use strict';

const { Server } = require('bindings')('server'); // bindings устанавливаются

const server = new Server();
server.Initialize();

server.Create();

server.StartListen();

server.HandleClients();