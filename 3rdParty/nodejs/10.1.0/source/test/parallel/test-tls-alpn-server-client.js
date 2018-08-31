'use strict';
const common = require('../common');

if (!common.hasCrypto)
  common.skip('missing crypto');

if (!process.features.tls_alpn) {
  common.skip(
    'Skipping because node compiled without ALPN feature of OpenSSL.');
}

const assert = require('assert');
const tls = require('tls');
const fixtures = require('../common/fixtures');

function loadPEM(n) {
  return fixtures.readKey(`${n}.pem`);
}

const serverIP = common.localhostIPv4;

function checkResults(result, expected) {
  assert.strictEqual(result.server.ALPN, expected.server.ALPN);
  assert.strictEqual(result.client.ALPN, expected.client.ALPN);
}

function runTest(clientsOptions, serverOptions, cb) {
  serverOptions.key = loadPEM('agent2-key');
  serverOptions.cert = loadPEM('agent2-cert');
  const results = [];
  let index = 0;
  const server = tls.createServer(serverOptions, function(c) {
    results[index].server = { ALPN: c.alpnProtocol };
  });

  server.listen(0, serverIP, function() {
    connectClient(clientsOptions);
  });

  function connectClient(options) {
    const opt = options.shift();
    opt.port = server.address().port;
    opt.host = serverIP;
    opt.rejectUnauthorized = false;

    results[index] = {};
    const client = tls.connect(opt, function() {
      results[index].client = { ALPN: client.alpnProtocol };
      client.destroy();
      if (options.length) {
        index++;
        connectClient(options);
      } else {
        server.close();
        cb(results);
      }
    });
  }

}

// Server: ALPN, Client: ALPN
function Test1() {
  const serverOptions = {
    ALPNProtocols: ['a', 'b', 'c'],
  };

  const clientsOptions = [{
    ALPNProtocols: ['a', 'b', 'c'],
  }, {
    ALPNProtocols: ['c', 'b', 'e'],
  }, {
    ALPNProtocols: ['first-priority-unsupported', 'x', 'y'],
  }];

  runTest(clientsOptions, serverOptions, function(results) {
    // 'a' is selected by ALPN
    checkResults(results[0],
                 { server: { ALPN: 'a' },
                   client: { ALPN: 'a' } });
    // 'b' is selected by ALPN
    checkResults(results[1],
                 { server: { ALPN: 'b' },
                   client: { ALPN: 'b' } });
    // nothing is selected by ALPN
    checkResults(results[2],
                 { server: { ALPN: false },
                   client: { ALPN: false } });
    // execute next test
    Test2();
  });
}

// Server: ALPN, Client: Nothing
function Test2() {
  const serverOptions = {
    ALPNProtocols: ['a', 'b', 'c'],
  };

  const clientsOptions = [{}, {}, {}];

  runTest(clientsOptions, serverOptions, function(results) {
    // nothing is selected by ALPN
    checkResults(results[0],
                 { server: { ALPN: false },
                   client: { ALPN: false } });
    // nothing is selected by ALPN
    checkResults(results[1],
                 { server: { ALPN: false },
                   client: { ALPN: false } });
    // nothing is selected by ALPN
    checkResults(results[2],
                 { server: { ALPN: false },
                   client: { ALPN: false } });
    // execute next test
    Test3();
  });
}

// Server: Nothing, Client: ALPN
function Test3() {
  const serverOptions = {};

  const clientsOptions = [{
    ALPNrotocols: ['a', 'b', 'c'],
  }, {
    ALPNProtocols: ['c', 'b', 'e'],
  }, {
    ALPNProtocols: ['first-priority-unsupported', 'x', 'y'],
  }];

  runTest(clientsOptions, serverOptions, function(results) {
    // nothing is selected
    checkResults(results[0], { server: { ALPN: false },
                               client: { ALPN: false } });
    // nothing is selected
    checkResults(results[1], { server: { ALPN: false },
                               client: { ALPN: false } });
    // nothing is selected
    checkResults(results[2],
                 { server: { ALPN: false },
                   client: { ALPN: false } });
    // execute next test
    Test4();
  });
}

// Server: Nothing, Client: Nothing
function Test4() {
  const serverOptions = {};

  const clientsOptions = [{}, {}, {}];

  runTest(clientsOptions, serverOptions, function(results) {
    // nothing is selected
    checkResults(results[0], { server: { ALPN: false },
                               client: { ALPN: false } });
    // nothing is selected
    checkResults(results[1], { server: { ALPN: false },
                               client: { ALPN: false } });
    // nothing is selected
    checkResults(results[2],
                 { server: { ALPN: false },
                   client: { ALPN: false } });
  });
}

Test1();
