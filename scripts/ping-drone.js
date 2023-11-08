#!/bin/node

const PORT = 13284
const dgram = require("dgram");
const sock = dgram.createSocket({ type: "udp4"});


// LMAO JUST @everyone :))))))


const SUBNET_MASK = 0xffffff00; // 255.255.255.0
const ROUTER_IP = 0xC0A85601; // 192.168.86.1

function calculateSubnetBits(subnetMask) {
    let i = 0;
    let j = 1;
    while((subnetMask & j) == 0) {
        j = j << 1;
        i++;
    }
    return i;
}

const subnetBits = calculateSubnetBits(SUBNET_MASK);
const maxIp = 1 << subnetBits;


let sockets = [];

// skip router (***.***.***.0|1) :)
for(let i = 2; i < maxIp; i ++) {
    const sock = dgram.createSocket({ type: "udp4"});
    sockets.push(sock);
    const address = (ROUTER_IP & (SUBNET_MASK)) | i;
    sock.send("ping", PORT, `${(address >> 24) & 0xff}.${(address >> 16) & 0xff}.${(address >> 8) & 0xff}.${(address >> 0) & 0xff}`);

    sock.once("message", (msg, droneInfo) => {
        console.log(`Drone found: ${droneInfo.address}`);
        process.exit(0);
    });
}

setTimeout(() => {
    sockets.forEach((socket) => {
        socket.close();
    });
    console.error(`404 drone not found. :(`);
    process.exit(-1);
}, 10000);

