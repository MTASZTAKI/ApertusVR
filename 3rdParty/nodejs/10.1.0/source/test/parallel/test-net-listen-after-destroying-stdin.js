'use strict';
// Just test that destroying stdin doesn't mess up listening on a server.
// This is a regression test for
// https://github.com/nodejs/node-v0.x-archive/issues/746.

const common = require('../common');
const net = require('net');

process.stdin.destroy();

const server = net.createServer(common.mustCall(function(socket) {
  console.log('accepted');
  socket.end();
  server.close();
}));


server.listen(0, function() {
  console.log('listening...');

  net.createConnection(this.address().port);
});
