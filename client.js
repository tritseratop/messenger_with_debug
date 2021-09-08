'use strict';

let readline = require('readline-sync');
const { Client } = require('bindings')('client');

const client = new Client();

const output = (msg) => {
    console.log("js callback");
    console.log(msg);
    
};

let input = function () {
    let res = "";
    res = readline.prompt();
    return res;
};

client.Initialize();
client.Create();
client.Connect();

//void async function () {
client.StartChating(input, output);
//}();

/*while (true) {
    client.Receive(output);
}*/

/*void async function () {
    await client.StartReceiving(output);
}();*/


//client.StartSending(input);