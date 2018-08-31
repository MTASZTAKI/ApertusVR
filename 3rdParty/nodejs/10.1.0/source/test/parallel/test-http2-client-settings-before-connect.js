'use strict';

const common = require('../common');
if (!common.hasCrypto)
  common.skip('missing crypto');
const h2 = require('http2');

const server = h2.createServer();

// we use the lower-level API here
server.on('stream', common.mustCall((stream, headers, flags) => {
  stream.respond();
  stream.end('ok');
}));
server.on('session', common.mustCall((session) => {
  session.on('remoteSettings', common.mustCall(2));
}));

server.listen(0, common.mustCall(() => {
  const client = h2.connect(`http://localhost:${server.address().port}`);

  [
    ['headerTableSize', -1, RangeError],
    ['headerTableSize', 2 ** 32, RangeError],
    ['initialWindowSize', -1, RangeError],
    ['initialWindowSize', 2 ** 32, RangeError],
    ['maxFrameSize', 1, RangeError],
    ['maxFrameSize', 2 ** 24, RangeError],
    ['maxConcurrentStreams', -1, RangeError],
    ['maxConcurrentStreams', 2 ** 31, RangeError],
    ['maxHeaderListSize', -1, RangeError],
    ['maxHeaderListSize', 2 ** 32, RangeError],
    ['enablePush', 'a', TypeError],
    ['enablePush', 1, TypeError],
    ['enablePush', 0, TypeError],
    ['enablePush', null, TypeError],
    ['enablePush', {}, TypeError]
  ].forEach(([name, value, errorType]) =>
    common.expectsError(
      () => client.settings({ [name]: value }),
      {
        code: 'ERR_HTTP2_INVALID_SETTING_VALUE',
        type: errorType
      }
    )
  );

  [1, true, {}, []].forEach((invalidCallback) =>
    common.expectsError(
      () => client.settings({}, invalidCallback),
      {
        type: TypeError,
        code: 'ERR_INVALID_CALLBACK',
        message: 'Callback must be a function'
      }
    )
  );

  client.settings({ maxFrameSize: 1234567 });

  const req = client.request();
  req.on('response', common.mustCall());
  req.resume();
  req.on('close', common.mustCall(() => {
    server.close();
    client.close();
  }));
}));
