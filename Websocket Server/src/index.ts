import fs from 'fs'
import https from 'https';
import WebSocket from 'ws';

const options = {
    cert: fs.readFileSync('/etc/letsencrypt/live/linux-vm-southeastasia-1.southeastasia.cloudapp.azure.com/cert.pem'),
    ca: fs.readFileSync('/etc/letsencrypt/live/linux-vm-southeastasia-1.southeastasia.cloudapp.azure.com/chain.pem'),
    key: fs.readFileSync('/etc/letsencrypt/live/linux-vm-southeastasia-1.southeastasia.cloudapp.azure.com/privkey.pem')
};

const server = https.createServer(options);
const wss = new WebSocket.Server({ server });

type messageData = {
    type: string,
    value: { [key: string]: any }
}
const clients: Set<WebSocket> = new Set();

// Broadcast messages to clients of a specific path
const broadcast = (data: any, clients: Set<WebSocket>) => {
    clients.forEach((client) => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(data);
        }
    });
};

const ping = (client: WebSocket): NodeJS.Timeout => {
    const data = { type: "ping" }
    client.send(JSON.stringify(data));
    return setTimeout(() => {
        client.terminate();
    }, 5000);
}

wss.on('connection', (ws, req) => {
    let isArduino = false;
    let pingTask: NodeJS.Timeout;
    let pongTimeout: NodeJS.Timeout;
    clients.add(ws);
    ws.on('message', (message) => {
        const rawMessage: messageData = JSON.parse(message.toString());
        if (rawMessage.type === "status" && rawMessage.value?.status === 1) {
            broadcast(message, clients)
            isArduino = true;
            console.log("Connected");
            pingTask = setInterval(() => { pongTimeout = ping(ws) }, 25000);
        } else if (rawMessage.type === "pong") {
            clearTimeout(pongTimeout)
        }
        else {
            broadcast(message, clients)
        }
    });

    // Handle client disconnection
    ws.on('close', () => {
        clients.delete(ws);
        if (isArduino) {
            const messageRaw: messageData = { type: "status", value: { status: 0 } };
            const msgDisconnect = JSON.stringify(messageRaw)
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
