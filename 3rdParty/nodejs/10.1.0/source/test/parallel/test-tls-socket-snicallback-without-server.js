'use strict';

// This is based on test-tls-securepair-fiftharg.js
// for the deprecated `tls.createSecurePair()` variant.

const common = require('../common');
if (!common.hasCrypto)
  common.skip('missing crypto');

const assert = require('assert');
const tls = require('tls');
const fixtures = require('../common/fixtures');
const makeDuplexPair = require('../common/duplexpair');

const { clientSide, serverSide } = makeDuplexPair();
new tls.TLSSocket(serverSide, {
  isServer: true,
  SNICallback: common.mustCall((servername, cb) => {
    assert.strictEqual(servername, 'www.google.com');
  })
});

// captured traffic from browser's request to https://www.google.com
const sslHello = fixtures.readSync('google_ssl_hello.bin');

clientSide.write(sslHello);
