'use strict';

let readline = require('readline-sync');
const areadline = require('readline');
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

async function inputCallback() {
    const rl = areadline.createInterface({
        input: process.stdin,
        output: process.stdout
    });
    return new Promise((resolve) => {
        rl.question("Enter the message:", (answer) => {
            rl.close();
            resolve(answer);
        });
    });
}

client.Initialize();
client.Create();
client.Connect();

//void async function () {
if (client.StartChating(output, inputCallback) == false) {
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