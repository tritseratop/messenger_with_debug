'use strict';

let readline = require('readline-sync');
readline.se
const { Client } = require('bindings')('client');

const client = new Client();

/*const output = (...args) => {
    console.log(...args);
};*/

const output = (msg) => {
    console.log(msg);
};

let input = function () {
    let res = "";
    res = readline.prompt({ prompt: ''});
    return res;
};

client.Initialize();
client.Create();
client.Connect();

//void async function () {
client.StartChating(output, input);
//}();

/*while (true) {
    client.Receive(output);
}*/

/*void async function () {
    await client.StartReceiving(output);
}();*/


//client.StartSending(input);