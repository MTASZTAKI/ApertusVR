'use strict';

const common = require('../common');
if (!common.hasCrypto)
  common.skip('missing crypto');
const assert = require('assert');
const http2 = require('http2');

const server = http2.createServer();
const data = Buffer.from([0x1, 0x2, 0x3, 0x4, 0x5]);

server.on('stream', common.mustCall((stream) => {
  stream.session.goaway(0, 0, data);
  stream.respond();
  stream.end();
}));

server.listen(0, () => {

  const client = http2.connect(`http://localhost:${server.address().port}`);
  client.once('goaway', common.mustCall((code, lastStreamID, buf) => {
    assert.deepStrictEqual(code, 0);
    assert.deepStrictEqual(lastStreamID, 1);
    assert.deepStrictEqual(data, buf);
    server.close();
  }));
  const req = client.request();
  req.resume();
  req.on('end', common.mustCall());
  req.on('close', common.mustCall());
  req.end();
});
