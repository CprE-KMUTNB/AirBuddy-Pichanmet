const https = require('https');
const http = require('http');
const WebSocket = require('ws');
const fs = require('fs');
// const options = {
//   cert: fs.readFileSync('<path_to_cert>'), // Path to the SSL certificate
//   key: fs.readFileSync('<path_to_key>') // Path to the SSL private key
// };
const wss = new WebSocket.Server({ noServer: true });
const subscribers = new Set();
wss.on('connection', (ws, req) => {
    console.log(req.authorization)
  if (!authenticate(req)) {
    ws.close(1008, 'Invalid username or password');
    return;
  }

  ws.on('message', (message) => {
    console.log(message)
    if (req.url === '/model') {
      broadcastToSubscribers('model', message);
    } else if (req.url === '/command') {
      broadcastToSubscribers('command', message);
    }
  });
  subscribers.add(ws);
  ws.on('close', () => {
    subscribers.delete(ws);
  });
});

function broadcastToSubscribers(endpoint, message) {
  subscribers.forEach((subscriber) => {
    if (subscriber.readyState === WebSocket.OPEN) {
      subscriber.send(message);
    }
  });
}
function authenticate(req) {
  const auth = req.headers.authorization;
  if (!auth) {
    return false;
  }
  const [username, password] = Buffer.from(auth.split(' ')[1], 'base64')
    .toString()
    .split(':');
  return username === 'esp' && password === 'esp32gaming';
}
const server = http.createServer((req, res) => {
  res.writeHead(200);
  res.end('WebSocket server is up and running!');
});
server.on('upgrade', (req, socket, head) => {
  wss.handleUpgrade(req, socket, head, (ws) => {
    wss.emit('connection', ws, req);
  });
});
server.listen(80, () => {
  console.log('WebSocket server is listening on port 80');
});