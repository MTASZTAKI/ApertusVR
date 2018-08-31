'use strict';

// Flags: --expose-internals

const common = require('../common');
common.crashOnUnhandledRejection();
const assert = require('assert');

const { ModuleWrap } = require('internal/test/binding');
const { getPromiseDetails, isPromise } = process.binding('util');
const setTimeoutAsync = require('util').promisify(setTimeout);

const foo = new ModuleWrap('export * from "bar"; 6;', 'foo');
const bar = new ModuleWrap('export const five = 5', 'bar');

(async () => {
  const promises = foo.link(() => setTimeoutAsync(1000).then(() => bar));
  assert.strictEqual(promises.length, 1);
  assert(isPromise(promises[0]));

  await Promise.all(promises);

  assert.strictEqual(getPromiseDetails(promises[0])[1], bar);

  foo.instantiate();

  assert.strictEqual(await foo.evaluate(-1, false), 6);
  assert.strictEqual(foo.namespace().five, 5);
})();
