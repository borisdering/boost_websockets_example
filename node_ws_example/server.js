const WebSocket = require('ws');

const wss = new WebSocket.Server({ port: 8080 });

let timer = null;

wss.on('connection', function connection(ws) {
    ws.on('message', function incoming(message) {
        console.log('received: %s', message);
    });

    timer = setInterval(() => {
        console.info('sending keep alive,...');
        ws.send('keep alive');
    }, 1000);
});

wss.on('close', () => {
    console.warn("connection has been closed.");
    clearInterval(timer);
});