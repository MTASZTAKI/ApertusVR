// Flags: --expose_internals
'use strict';
require('../common');
const assert = require('assert');
const BufferList = require('internal/streams/buffer_list');

// Test empty buffer list.
const emptyList = new BufferList();

emptyList.shift();
assert.deepStrictEqual(emptyList, new BufferList());

assert.strictEqual(emptyList.join(','), '');

assert.deepStrictEqual(emptyList.concat(0), Buffer.alloc(0));

const buf = Buffer.from('foo');

// Test buffer list with one element.
const list = new BufferList();
list.push(buf);

const copy = list.concat(3);

assert.notStrictEqual(copy, buf);
assert.deepStrictEqual(copy, buf);

assert.strictEqual(list.join(','), 'foo');

const shifted = list.shift();
assert.strictEqual(shifted, buf);
assert.deepStrictEqual(list, new BufferList());
