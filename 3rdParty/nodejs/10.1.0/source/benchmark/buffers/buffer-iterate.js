'use strict';
const SlowBuffer = require('buffer').SlowBuffer;
const common = require('../common.js');
const assert = require('assert');

const bench = common.createBenchmark(main, {
  size: [16, 512, 1024, 4096, 16386],
  type: ['fast', 'slow'],
  method: ['for', 'forOf', 'iterator'],
  n: [1e3]
});

const methods = {
  'for': benchFor,
  'forOf': benchForOf,
  'iterator': benchIterator
};

function main({ size, type, method, n }) {
  const clazz = type === 'fast' ? Buffer : SlowBuffer;
  const buffer = new clazz(size);
  buffer.fill(0);
  const fn = methods[method || 'for'];

  bench.start();
  fn(buffer, n);
  bench.end(n);
}

function benchFor(buffer, n) {
  for (var k = 0; k < n; k++) {
    for (var i = 0; i < buffer.length; i++) {
      assert(buffer[i] === 0);
    }
  }
}

function benchForOf(buffer, n) {
  for (var k = 0; k < n; k++) {
    for (const b of buffer) {
      assert(b === 0);
    }
  }
}

function benchIterator(buffer, n) {
  for (var k = 0; k < n; k++) {
    const iter = buffer[Symbol.iterator]();
    var cur = iter.next();

    while (!cur.done) {
      assert(cur.value === 0);
      cur = iter.next();
    }

  }
}
