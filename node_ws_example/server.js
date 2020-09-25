const WebSocket = require('ws');

const wss = new WebSocket.Server({ port: 8080 });

wss.on('connection', function connection(ws) {
    ws.on('message', function incoming(message) {
        console.log('received: %s', message);
    });

    setInterval(() => {
        console.info('sending keep alive,...');
        ws.send('keep alive');
        ws.connec
    }, 1000);
});

wss.on('close', () => {
    console.warn("connection has been closed.");
});