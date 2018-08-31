// Load required modules
var http = require("http");                                   // http server core module
var https = require("https");
var express = require("express");                             // web framework external module
var serveStatic = require('serve-static');                    // serve static files
var socketIo = require("socket.io");                          // web socket external module
var fs = require('fs');
var easyrtc = require("./easyrtc/easyrtc_server.js");         // EasyRTC external module

const pkey = fs.readFileSync('./ssl/key.pem'),
    pcert = fs.readFileSync('./ssl/cert.pem'),
    options = { key: pkey, cert: pcert, passphrase: '123456789' };

// Set process name
process.title = "node-easyrtc";

// Setup and configure Express http server. Expect a subfolder called "static" to be the web root.
var app = express();
app.use(serveStatic('static', { 'index': ['index.html'] }));

var sslServer = https.createServer(options, app).listen(443);
console.log("The HTTPS server is up and running");

// Start Socket.io so it attaches itself to Express server
var socketServer = socketIo.listen(sslServer, { "log level": 1 });
console.log("WebSocket Secure server is up and running.");

easyrtc.setOption("logLevel", "debug");

// Overriding the default easyrtcAuth listener, only so we can directly access its callback
easyrtc.events.on("easyrtcAuth", function (socket, easyrtcid, msg, socketCallback, callback) {
    easyrtc.events.defaultListeners.easyrtcAuth(socket, easyrtcid, msg, socketCallback, function (err, connectionObj) {
        if (err || !msg.msgData || !msg.msgData.credential || !connectionObj) {
            callback(err, connectionObj);
            return;
        }
        connectionObj.setField("credential", msg.msgData.credential, { "isShared": false });
        console.log("[" + easyrtcid + "] Credential saved!", connectionObj.getFieldValueSync("credential"));
        callback(err, connectionObj);
    });
});

// To test, lets print the credential to the console for every room join!
easyrtc.events.on("roomJoin", function (connectionObj, roomName, roomParameter, callback) {
    console.log("[" + connectionObj.getEasyrtcid() + "] Credential retrieved!", connectionObj.getFieldValueSync("credential"));
    easyrtc.events.defaultListeners.roomJoin(connectionObj, roomName, roomParameter, callback);
});

easyrtc.events.on("roomCreate", function (appObj, creatorConnectionObj, roomName, roomOptions, callback) {
    console.log("roomCreate fired! Trying to create: " + roomName);
    appObj.events.defaultListeners.roomCreate(appObj, creatorConnectionObj, roomName, roomOptions, callback);
});

// Start EasyRTC server
var rtc = easyrtc.listen(app, socketServer, null, function (err, rtcRef) {
    if (err) {
        console.log(err);
    }
    console.log("Initiated");
});
