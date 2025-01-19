#!/bin/env/ node

const http = require('http');
const fs = require('fs');
const socketio = require('socket.io');
const OSC = require('osc-js');

// Configuration for OSC
const UDP_SEND_PORT = 9997; // Default port for /mode
const SPECIAL_PORT = 6666;  // Port for /stop and /play
const OSC_ADDRESS = "/mode"; // Default OSC address
const STOP_ADDRESS = "/stop";
const PLAY_ADDRESS = "/play";

// Create HTTP server
const httpServer = http.createServer((req, res) => {
    fs.readFile('./interface.html', (err, data) => {
        if (err) {
            res.writeHead(500);
            return res.end('Error loading interface.html');
        }

        res.writeHead(200, { 'Content-Type': 'text/html' });
        res.end(data);
    });
});

// Attach Socket.IO to the HTTP server
const io = socketio(httpServer);

// Configure OSC
const osc = new OSC({
    plugin: new OSC.DatagramPlugin({
        open: {
            host: '0.0.0.0', // Local IP to listen for incoming messages
            port: 9998,      // UDP receive port
        },
        send: {
            host: '127.0.0.1', // Target IP for outgoing messages
        },
    }),
});

osc.open(); // Start OSC server and client

// Function to send /play OSC message
const triggerPlay = () => {
    const playMessage = new OSC.Message(PLAY_ADDRESS, true); // Boolean true
    osc.send(playMessage, { port: SPECIAL_PORT });
    console.log(`OSC message triggered: ${PLAY_ADDRESS} true`);
};

// Handle WebSocket connections
io.on('connection', (socket) => {
    console.log('A client connected');

    socket.on('buttonClicked', (value) => {
        console.log(`Button clicked with value: ${value}`);

        if (value === 0) {
            // Stop button
            const stopMessage = new OSC.Message(STOP_ADDRESS, true); // Boolean true
            osc.send(stopMessage, { port: SPECIAL_PORT });
            console.log(`OSC message sent: ${STOP_ADDRESS} true`);
        } else if (value === 6) {
            // Play button
            triggerPlay(); // Use the shared function to send /play
        } else {
            // Other buttons
            const message = new OSC.Message(OSC_ADDRESS, value); // Integer value
            osc.send(message, { port: UDP_SEND_PORT });
            console.log(`OSC message sent: ${OSC_ADDRESS} ${value}`);
        }
    });

    socket.on('disconnect', () => {
        console.log('A client disconnected');
    });
});

// Start the HTTP server
const HTTP_PORT = 8080;
httpServer.listen(HTTP_PORT, () => {
    console.log(`HTTP server is running on http://localhost:${HTTP_PORT}`);
});
