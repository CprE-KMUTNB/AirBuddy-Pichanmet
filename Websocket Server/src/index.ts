import fs from 'fs' // Import the fs module for file operations
import https from 'https'; // Import the https module for creating an HTTPS server
import WebSocket from 'ws'; // Import the WebSocket module for creating a WebSocket server

const options = {
    cert: fs.readFileSync('/etc/letsencrypt/live/linux-vm-southeastasia-1.southeastasia.cloudapp.azure.com/cert.pem'), // Read the SSL certificate file
    ca: fs.readFileSync('/etc/letsencrypt/live/linux-vm-southeastasia-1.southeastasia.cloudapp.azure.com/chain.pem'), // Read the certificate authority file
    key: fs.readFileSync('/etc/letsencrypt/live/linux-vm-southeastasia-1.southeastasia.cloudapp.azure.com/privkey.pem') // Read the private key file
};

const server = https.createServer(options); // Create an HTTPS server using the provided options
const wss = new WebSocket.Server({ server }); // Create a WebSocket server using the HTTPS server

type messageData = {
    type: string,
    value: { [key: string]: any }
}

const clients: Set<WebSocket> = new Set(); // Create an empty set to hold the connected WebSocket clients

// Function to broadcast messages to all connected clients
const broadcast = (data: any, clients: Set<WebSocket>) => {
    clients.forEach((client) => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(data); // Send the data to the client if the connection is open
        }
    });
};

// Function to send a ping message to a client and set a timeout to terminate the connection if no pong message is received
const ping = (client: WebSocket): NodeJS.Timeout => {
    const data = { type: "ping" }
    client.send(JSON.stringify(data)); // Send the ping message to the client
    return setTimeout(() => {
        client.terminate(); // Terminate the connection if no pong message is received within 5 seconds
    }, 5000);
}

// Event listener for new WebSocket connections
wss.on('connection', (ws, req) => {
    let isArduino = false; // Flag to indicate if the connected client is an Arduino
    let pingTask: NodeJS.Timeout; // Reference to the ping interval timer
    let pongTimeout: NodeJS.Timeout; // Reference to the pong timeout timer
    clients.add(ws); // Add the connected WebSocket client to the set

    // Event listener for incoming messages from the client
    ws.on('message', (message) => {
        const rawMessage: messageData = JSON.parse(message.toString()); // Parse the incoming message as JSON
        if (rawMessage.type === "status" && rawMessage.value?.status === 1) { // Check if the message is a status message and the status value is 1
            broadcast(message, clients); // Broadcast the message to all clients
            isArduino = true; // Set the isArduino flag to true
            console.log("Connected"); // Log the connection status
            pingTask = setInterval(() => { pongTimeout = ping(ws); }, 30000); // Send ping messages to the client every 30 seconds
        } else if (rawMessage.type === "pong") { // Check if the message is a pong message
            clearTimeout(pongTimeout); // Clear the pong timeout timer
        }
        else {
            broadcast(message, clients); // Broadcast the message to all clients
        }
    });

    // Event listener for client disconnection
    ws.on('close', () => {
        clients.delete(ws); // Remove the disconnected client from the set
        if (isArduino) { // Check if the disconnected client was an Arduino
            const messageRaw: messageData = { type: "status", value: { status: 0 } }; // Create a status message with status value 0
            const msgDisconnect = JSON.stringify(messageRaw); // Convert the message to a JSON string
            broadcast(msgDisconnect, clients); // Broadcast the disconnect message to all clients
            clearInterval(pingTask); // Clear the ping interval timer
            console.log("Disconnected"); // Log the disconnection status
        }
    });
});

// Start the server
server.listen(8080, () => {
    console.log('WebSocket server started on port 8080'); // Log the server start status
});