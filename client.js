'use strict';

let readline = require('readline-sync');
const { Client } = require('bindings')('client');

const client = new Client();

/*const output = (...args) => {
    console.log(...args);
};*/

const output = (msg) => {
    console.log("Js callback output");
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
if (client.StartChating(output, input) == false) {
    console.log("The end");
}
//}();

/*while (true) {
    client.Receive(output);
}*/

/*void async function () {
    await client.StartReceiving(output);
}();*/


//client.StartSending(input);