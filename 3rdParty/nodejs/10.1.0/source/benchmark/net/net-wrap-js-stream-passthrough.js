// test the speed of .pipe() with JSStream wrapping for PassThrough streams
'use strict';

const common = require('../common.js');
const { PassThrough } = require('stream');

const bench = common.createBenchmark(main, {
  len: [102400, 1024 * 1024 * 16],
  type: ['utf', 'asc', 'buf'],
  dur: [5],
}, {
  flags: ['--expose-internals']
});

var chunk;
var encoding;

function main({ dur, len, type }) {
  // Can only require internals inside main().
  const JSStreamWrap = require('internal/wrap_js_stream');

  switch (type) {
    case 'buf':
      chunk = Buffer.alloc(len, 'x');
      break;
    case 'utf':
      encoding = 'utf8';
      chunk = 'ü'.repeat(len / 2);
      break;
    case 'asc':
      encoding = 'ascii';
      chunk = 'x'.repeat(len);
      break;
    default:
      throw new Error(`invalid type: ${type}`);
  }

  const reader = new Reader();
  const writer = new Writer();

  // the actual benchmark.
  const fakeSocket = new JSStreamWrap(new PassThrough());
  bench.start();
  reader.pipe(fakeSocket);
  fakeSocket.pipe(writer);

  setTimeout(function() {
    const bytes = writer.received;
    const gbits = (bytes * 8) / (1024 * 1024 * 1024);
    bench.end(gbits);
    process.exit(0);
  }, dur * 1000);
}

function Writer() {
  this.received = 0;
  this.writable = true;
}

Writer.prototype.write = function(chunk, encoding, cb) {
  this.received += chunk.length;

  if (typeof encoding === 'function')
    encoding();
  else if (typeof cb === 'function')
    cb();

  return true;
};

// doesn't matter, never emits anything.
Writer.prototype.on = function() {};
Writer.prototype.once = function() {};
Writer.prototype.emit = function() {};
Writer.prototype.prependListener = function() {};


function flow() {
  const dest = this.dest;
  const res = dest.write(chunk, encoding);
  if (!res)
    dest.once('drain', this.flow);
  else
    process.nextTick(this.flow);
}

function Reader() {
  this.flow = flow.bind(this);
  this.readable = true;
}

Reader.prototype.pipe = function(dest) {
  this.dest = dest;
  this.flow();
  return dest;
};
