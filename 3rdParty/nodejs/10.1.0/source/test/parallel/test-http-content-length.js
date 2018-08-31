'use strict';
require('../common');
const assert = require('assert');
const http = require('http');
const Countdown = require('../common/countdown');

const expectedHeadersMultipleWrites = {
  'connection': 'close',
  'transfer-encoding': 'chunked',
};

const expectedHeadersEndWithData = {
  'connection': 'close',
  'content-length': String('hello world'.length)
};

const expectedHeadersEndNoData = {
  'connection': 'close',
  'content-length': '0',
};


const countdown = new Countdown(3, () => server.close());

const server = http.createServer(function(req, res) {
  res.removeHeader('Date');

  switch (req.url.substr(1)) {
    case 'multiple-writes':
      assert.deepStrictEqual(req.headers, expectedHeadersMultipleWrites);
      res.write('hello');
      res.end('world');
      break;
    case 'end-with-data':
      assert.deepStrictEqual(req.headers, expectedHeadersEndWithData);
      res.end('hello world');
      break;
    case 'empty':
      assert.deepStrictEqual(req.headers, expectedHeadersEndNoData);
      res.end();
      break;
    default:
      throw new Error('Unreachable');
  }

  countdown.dec();
});

server.listen(0, function() {
  let req;

  req = http.request({
    port: this.address().port,
    method: 'POST',
    path: '/multiple-writes'
  });
  req.removeHeader('Date');
  req.removeHeader('Host');
  req.write('hello ');
  req.end('world');
  req.on('response', function(res) {
    assert.deepStrictEqual(res.headers, expectedHeadersMultipleWrites);
  });

  req = http.request({
    port: this.address().port,
    method: 'POST',
    path: '/end-with-data'
  });
  req.removeHeader('Date');
  req.removeHeader('Host');
  req.end('hello world');
  req.on('response', function(res) {
    assert.deepStrictEqual(res.headers, expectedHeadersEndWithData);
  });

  req = http.request({
    port: this.address().port,
    method: 'POST',
    path: '/empty'
  });
  req.removeHeader('Date');
  req.removeHeader('Host');
  req.end();
  req.on('response', function(res) {
    assert.deepStrictEqual(res.headers, expectedHeadersEndNoData);
  });

});
