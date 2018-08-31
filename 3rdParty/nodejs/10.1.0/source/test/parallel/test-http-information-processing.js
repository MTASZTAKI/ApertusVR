'use strict';
require('../common');
const assert = require('assert');
const http = require('http');
const Countdown = require('../common/countdown');

const test_res_body = 'other stuff!\n';
const countdown = new Countdown(3, () => server.close());

const server = http.createServer((req, res) => {
  console.error('Server sending informational message #1...');
  res.writeProcessing();
  console.error('Server sending informational message #2...');
  res.writeProcessing();
  console.error('Server sending full response...');
  res.writeHead(200, {
    'Content-Type': 'text/plain',
    'ABCD': '1'
  });
  res.end(test_res_body);
});

server.listen(0, function() {
  const req = http.request({
    port: this.address().port,
    path: '/world'
  });
  req.end();
  console.error('Client sending request...');

  let body = '';

  req.on('information', function(res) {
    console.error('Client got 102 Processing...');
    countdown.dec();
  });

  req.on('response', function(res) {
    assert.strictEqual(countdown.remaining, 1,
                       'Full response received before all 102 Processing');
    assert.strictEqual(200, res.statusCode,
                       `Final status code was ${res.statusCode}, not 200.`);
    res.setEncoding('utf8');
    res.on('data', function(chunk) { body += chunk; });
    res.on('end', function() {
      console.error('Got full response.');
      assert.strictEqual(body, test_res_body);
      assert.ok('abcd' in res.headers);
      countdown.dec();
    });
  });
});
