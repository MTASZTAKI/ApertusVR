'use strict';

require('../common');
const assert = require('assert');

/*
 * We need to check the following things:
 *  - We are correctly resolving big endian (doesn't mean anything for 8 bit)
 *  - Correctly resolving little endian (doesn't mean anything for 8 bit)
 *  - Correctly using the offsets
 *  - Correctly interpreting values that are beyond the signed range as unsigned
 */

{ // OOB
  const data = Buffer.alloc(8);
  ['UInt8', 'UInt16BE', 'UInt16LE', 'UInt32BE', 'UInt32LE'].forEach((fn) => {

    // Verify that default offset works fine.
    data[`write${fn}`](23, undefined);
    data[`write${fn}`](23);

    ['', '0', null, {}, [], () => {}, true, false].forEach((o) => {
      assert.throws(
        () => data[`write${fn}`](23, o),
        { code: 'ERR_INVALID_ARG_TYPE' });
    });

    [NaN, Infinity, -1, 1.01].forEach((o) => {
      assert.throws(
        () => data[`write${fn}`](23, o),
        { code: 'ERR_OUT_OF_RANGE' });
    });
  });
}

{ // Test 8 bit
  const data = Buffer.alloc(4);

  data.writeUInt8(23, 0);
  data.writeUInt8(23, 1);
  data.writeUInt8(23, 2);
  data.writeUInt8(23, 3);
  assert.ok(data.equals(new Uint8Array([23, 23, 23, 23])));

  data.writeUInt8(23, 0);
  data.writeUInt8(23, 1);
  data.writeUInt8(23, 2);
  data.writeUInt8(23, 3);
  assert.ok(data.equals(new Uint8Array([23, 23, 23, 23])));

  data.writeUInt8(255, 0);
  assert.strictEqual(data[0], 255);

  data.writeUInt8(255, 0);
  assert.strictEqual(data[0], 255);
}

// Test 16 bit
{
  let value = 0x2343;
  const data = Buffer.alloc(4);

  data.writeUInt16BE(value, 0);
  assert.ok(data.equals(new Uint8Array([0x23, 0x43, 0, 0])));

  data.writeUInt16BE(value, 1);
  assert.ok(data.equals(new Uint8Array([0x23, 0x23, 0x43, 0])));

  data.writeUInt16BE(value, 2);
  assert.ok(data.equals(new Uint8Array([0x23, 0x23, 0x23, 0x43])));

  data.writeUInt16LE(value, 0);
  assert.ok(data.equals(new Uint8Array([0x43, 0x23, 0x23, 0x43])));

  data.writeUInt16LE(value, 1);
  assert.ok(data.equals(new Uint8Array([0x43, 0x43, 0x23, 0x43])));

  data.writeUInt16LE(value, 2);
  assert.ok(data.equals(new Uint8Array([0x43, 0x43, 0x43, 0x23])));

  value = 0xff80;
  data.writeUInt16LE(value, 0);
  assert.ok(data.equals(new Uint8Array([0x80, 0xff, 0x43, 0x23])));

  data.writeUInt16BE(value, 0);
  assert.ok(data.equals(new Uint8Array([0xff, 0x80, 0x43, 0x23])));
}

// Test 32 bit
{
  const data = Buffer.alloc(6);
  const value = 0xe7f90a6d;

  data.writeUInt32BE(value, 0);
  assert.ok(data.equals(new Uint8Array([0xe7, 0xf9, 0x0a, 0x6d, 0, 0])));

  data.writeUInt32BE(value, 1);
  assert.ok(data.equals(new Uint8Array([0xe7, 0xe7, 0xf9, 0x0a, 0x6d, 0])));

  data.writeUInt32BE(value, 2);
  assert.ok(data.equals(new Uint8Array([0xe7, 0xe7, 0xe7, 0xf9, 0x0a, 0x6d])));

  data.writeUInt32LE(value, 0);
  assert.ok(data.equals(new Uint8Array([0x6d, 0x0a, 0xf9, 0xe7, 0x0a, 0x6d])));

  data.writeUInt32LE(value, 1);
  assert.ok(data.equals(new Uint8Array([0x6d, 0x6d, 0x0a, 0xf9, 0xe7, 0x6d])));

  data.writeUInt32LE(value, 2);
  assert.ok(data.equals(new Uint8Array([0x6d, 0x6d, 0x6d, 0x0a, 0xf9, 0xe7])));
}

// Test UInt
{
  const data = Buffer.alloc(8);
  let val = 0x100;

  // Check byteLength.
  ['writeUIntBE', 'writeUIntLE'].forEach((fn) => {

    // Verify that default offset & byteLength works fine.
    data[fn](undefined, undefined);
    data[fn](undefined);
    data[fn]();

    ['', '0', null, {}, [], () => {}, true, false].forEach((bl) => {
      assert.throws(
        () => data[fn](23, 0, bl),
        { code: 'ERR_INVALID_ARG_TYPE' });
    });

    [Infinity, -1].forEach((offset) => {
      assert.throws(
        () => data[fn](23, 0, offset),
        {
          code: 'ERR_OUT_OF_RANGE',
          message: 'The value of "byteLength" is out of range. ' +
                   `It must be >= 1 and <= 6. Received ${offset}`
        }
      );
    });

    [NaN, 1.01].forEach((byteLength) => {
      assert.throws(
        () => data[fn](42, 0, byteLength),
        {
          code: 'ERR_OUT_OF_RANGE',
          name: 'RangeError [ERR_OUT_OF_RANGE]',
          message: 'The value of "byteLength" is out of range. ' +
                   `It must be an integer. Received ${byteLength}`
        });
    });
  });

  // Test 1 to 6 bytes.
  for (let i = 1; i < 6; i++) {
    ['writeUIntBE', 'writeUIntLE'].forEach((fn) => {
      assert.throws(() => {
        data[fn](val, 0, i);
      }, {
        code: 'ERR_OUT_OF_RANGE',
        name: 'RangeError [ERR_OUT_OF_RANGE]',
        message: 'The value of "value" is out of range. ' +
                 `It must be >= 0 and <= ${val - 1}. Received ${val}`
      });

      ['', '0', null, {}, [], () => {}, true, false].forEach((o) => {
        assert.throws(
          () => data[fn](23, o, i),
          {
            code: 'ERR_INVALID_ARG_TYPE',
            name: 'TypeError [ERR_INVALID_ARG_TYPE]'
          });
      });

      [Infinity, -1, -4294967295].forEach((offset) => {
        assert.throws(
          () => data[fn](val - 1, offset, i),
          {
            code: 'ERR_OUT_OF_RANGE',
            name: 'RangeError [ERR_OUT_OF_RANGE]',
            message: 'The value of "offset" is out of range. ' +
                     `It must be >= 0 and <= ${8 - i}. Received ${offset}`
          });
      });

      [NaN, 1.01].forEach((offset) => {
        assert.throws(
          () => data[fn](val - 1, offset, i),
          {
            code: 'ERR_OUT_OF_RANGE',
            name: 'RangeError [ERR_OUT_OF_RANGE]',
            message: 'The value of "offset" is out of range. ' +
                     `It must be an integer. Received ${offset}`
          });
      });
    });

    val *= 0x100;
  }
}
