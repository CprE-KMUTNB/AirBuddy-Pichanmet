"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const fs_1 = __importDefault(require("fs"));
const https_1 = __importDefault(require("https"));
const ws_1 = __importDefault(require("ws"));
const options = {
    cert: fs_1.default.readFileSync('/etc/letsencrypt/live/linux-vm-southeastasia-1.southeastasia.cloudapp.azure.com/cert.pem'),
    ca: fs_1.default.readFileSync('/etc/letsencrypt/live/linux-vm-southeastasia-1.southeastasia.cloudapp.azure.com/chain.pem'),
    key: fs_1.default.readFileSync('/etc/letsencrypt/live/linux-vm-southeastasia-1.southeastasia.cloudapp.azure.com/privkey.pem')
};
const server = https_1.default.createServer(options);
const wss = new ws_1.default.Server({ server });
const clients = new Set();
// Broadcast messages to clients of a specific path
const broadcast = (data, clients) => {
    clients.forEach((client) => {
        if (client.readyState === ws_1.default.OPEN) {
            client.send(data);
        }
    });
};
const ping = (client) => {
    const data = { type: "ping" };
    client.send(JSON.stringify(data));
    return setTimeout(() => {
        client.terminate();
    }, 5000);
};
wss.on('connection', (ws, req) => {
    let isArduino = false;
    let pingTask;
    let pongTimeout;
    clients.add(ws);
    ws.on('message', (message) => {
        const rawMessage = JSON.parse(message.toString());
        if (rawMessage.type === "status" && rawMessage.value?.status === 1) {
            broadcast(message, clients);
            isArduino = true;
            console.log("Connected");
            pingTask = setInterval(() => { pongTimeout = ping(ws); }, 25000);
        }
        else if (rawMessage.type === "pong") {
            clearTimeout(pongTimeout);
        }
        else {
            broadcast(message, clients);
        }
    });
    // Handle client disconnection
    ws.on('close', () => {
        clients.delete(ws);
        if (isArduino) {
            const messageRaw = { type: "status", value: { status: 0 } };
            const msgDisconnect = JSON.stringify(messageRaw);
            broadcast(msgDisconnect, clients);
            clearInterval(pingTask);
            console.log("Disconnected");
        }
    });
});
// Start the server
server.listen(8080, () => {
    console.log('WebSocket server started on port 8080');
});
