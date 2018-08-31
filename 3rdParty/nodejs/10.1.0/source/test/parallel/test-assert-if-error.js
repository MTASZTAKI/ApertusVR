'use strict';

require('../common');
const assert = require('assert').strict;
/* eslint-disable no-restricted-properties */

// Test that assert.ifError has the correct stack trace of both stacks.

let err;
// Create some random error frames.
(function a() {
  (function b() {
    (function c() {
      err = new Error('test error');
    })();
  })();
})();

const msg = err.message;
const stack = err.stack;

(function x() {
  (function y() {
    (function z() {
      let threw = false;
      try {
        assert.ifError(err);
      } catch (e) {
        assert.equal(e.message, 'ifError got unwanted exception: test error');
        assert.equal(err.message, msg);
        assert.equal(e.actual, err);
        assert.equal(e.actual.stack, stack);
        assert.equal(e.expected, null);
        assert.equal(e.operator, 'ifError');
        threw = true;
      }
      assert(threw);
    })();
  })();
})();

assert.throws(
  () => assert.ifError(new TypeError()),
  {
    message: 'ifError got unwanted exception: TypeError'
  }
);

assert.throws(
  () => assert.ifError({ stack: false }),
  {
    message: 'ifError got unwanted exception: { stack: false }'
  }
);

assert.throws(
  () => assert.ifError({ constructor: null, message: '' }),
  {
    message: 'ifError got unwanted exception: '
  }
);

assert.throws(
  () => { assert.ifError(false); },
  {
    message: 'ifError got unwanted exception: false'
  }
);

// Should not throw.
assert.ifError(null);
assert.ifError();
assert.ifError(undefined);

// https://github.com/nodejs/node-v0.x-archive/issues/2893
{
  let threw = false;
  try {
    // eslint-disable-next-line no-restricted-syntax
    assert.throws(() => {
      assert.ifError(null);
    });
  } catch (e) {
    threw = true;
    assert.strictEqual(e.message, 'Missing expected exception.');
    assert(!e.stack.includes('throws'), e.stack);
  }
  assert(threw);
}
