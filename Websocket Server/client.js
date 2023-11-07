const WebSocket = require('ws');
const ws = new WebSocket('ws://127.0.0.1/command');
const username = '';
const password = 'esp32gaming';
ws.on('open', () => {
    ws.send('Handshake');
    ws.send('Handshake');
    ws.send('Handshake');
    ws.send('Handshake');
    ws.send('Handshake');
    ws.send('Handshake');
    ws.send('Handshake');
    ws.send('Handshake');
    console.log("OK");
});
ws.on('message', (message) => {
    console.log('Received message:', message);
});
ws.on('upgrade', (res) => {
    res.headers.authorization =  'Basic ' + Buffer.from(username + ':' + password).toString('base64');
});
ws.on('error', (error) => {
    console.error('WebSocket error:', error);
});

ws.on('close', (code, reason) => {
    console.log('WebSocket closed:', code, reason);
})


setTimeout(() => {}, 10000);