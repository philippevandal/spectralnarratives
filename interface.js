const http = require('http');
const fs = require('fs');
const socketio = require('socket.io');
const dgram = require('dgram');

const UDP_RECEIVE_PORT = 9998;
const UDP_SEND_PORT = 9997;
const OSC_ADDRESS = "OSCdevice:/mode/";

// Create HTTP server
const http_server = http.createServer((req, res) => {
    // Serve the HTML interface
    fs.readFile(__dirname + '/interface.html', (err, data) => {
        if (err) {
            res.writeHead(500);
            return res.end('Error loading interface.html');
        }

        res.writeHead(200, { 'Content-Type': 'text/html' });
        res.end(data);
    });
});

// Attach Socket.IO to the HTTP server
const io = socketio(http_server);

// Handle WebSocket connections
io.on('connection', (socket) => {
    console.log('A client connected');

    socket.on('buttonClicked', (value) => {
        console.log(`Button clicked with value: ${value}`);

        // Send OSC message
        const udpServer = dgram.createSocket('udp4');
        const messageBuffer = Buffer.from(`${OSC_ADDRESS} ${value}`);
        udpServer.send(messageBuffer, 0, messageBuffer.length, UDP_SEND_PORT, '127.0.0.1', (err) => {
            if (err) {
                console.error(`Error sending OSC message: ${err.message}`);
            } else {
                console.log(`OSC message sent: ${OSC_ADDRESS} ${value}`);
            }
            udpServer.close();
        });
    });

    socket.on('disconnect', () => {
        console.log('A client disconnected');
    });
});

// Start the HTTP server
const HTTP_PORT = 8080;
http_server.listen(HTTP_PORT, () => {
    console.log(`HTTP server is running on http://localhost:${HTTP_PORT}`);
});

// UDP server setup (if needed)
const udpReceiver = dgram.createSocket('udp4');
udpReceiver.on('message', (msg, rinfo) => {
    console.log(`UDP server received: ${msg} from ${rinfo.address}:${rinfo.port}`);
});
udpReceiver.bind(UDP_RECEIVE_PORT, '0.0.0.0');

// const http = require('http');
// const socketIo = require('socket.io');
// const dgram = require('dgram');

// // HTTP Server setup
// const httpServer = http.createServer();
// const io = socketIo(httpServer, {
//     cors: {
//         origin: "*"
//     }
// });

// const HTTP_PORT = 8080;
// const UDP_SEND_PORT = 9997;
// const UDP_RECEIVE_PORT = 9998;
// const OSC_ADDRESS = "OSCdevice:/mode/";

// // UDP Server setup
// const udpServer = dgram.createSocket('udp4');
// udpServer.bind(UDP_RECEIVE_PORT, '0.0.0.0', () => {
//     console.log(`UDP server is listening on port ${UDP_RECEIVE_PORT}`);
// });

// // HTTP Server listens on port 8080
// httpServer.listen(HTTP_PORT, () => {
//     console.log(`HTTP server is listening on port ${HTTP_PORT}`);
// });

// // Socket.IO connection
// io.on('connection', (socket) => {
//     console.log('A client connected');

//     socket.on('buttonClicked', (value) => {
//         console.log(`Button clicked with value: ${value}`);
//         sendOscMessage(parseInt(value, 10));
//     });

//     socket.on('disconnect', () => {
//         console.log('A client disconnected');
//     });
// });

// // Function to send OSC message
// function sendOscMessage(value) {
//     const oscMessage = `${OSC_ADDRESS} ${value}`;
//     const messageBuffer = Buffer.from(oscMessage);

//     udpServer.send(messageBuffer, 0, messageBuffer.length, UDP_SEND_PORT, '127.0.0.1', (err) => {
//         if (err) {
//             console.error(`Error sending OSC message: ${err.message}`);
//         } else {
//             console.log(`OSC message sent: ${oscMessage}`);
//         }
//     });
// }