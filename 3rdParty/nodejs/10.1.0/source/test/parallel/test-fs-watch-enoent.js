'use strict';

// This verifies the error thrown by fs.watch.

const common = require('../common');
const assert = require('assert');
const fs = require('fs');
const tmpdir = require('../common/tmpdir');
const path = require('path');
const nonexistentFile = path.join(tmpdir.path, 'non-existent');
const uv = process.binding('uv');

tmpdir.refresh();

{
  const validateError = (err) => {
    assert.strictEqual(err.path, nonexistentFile);
    assert.strictEqual(err.filename, nonexistentFile);
    assert.strictEqual(err.syscall, 'watch');
    if (err.code === 'ENOENT') {
      assert.strictEqual(
        err.message,
        `ENOENT: no such file or directory, watch '${nonexistentFile}'`);
      assert.strictEqual(err.errno, uv.UV_ENOENT);
      assert.strictEqual(err.code, 'ENOENT');
    } else {  // AIX
      assert.strictEqual(
        err.message,
        `ENODEV: no such device, watch '${nonexistentFile}'`);
      assert.strictEqual(err.errno, uv.UV_ENODEV);
      assert.strictEqual(err.code, 'ENODEV');
    }
    return true;
  };

  assert.throws(
    () => fs.watch(nonexistentFile, common.mustNotCall()),
    validateError
  );
}

{
  const file = path.join(tmpdir.path, 'file-to-watch');
  fs.writeFileSync(file, 'test');
  const watcher = fs.watch(file, common.mustNotCall());

  const validateError = (err) => {
    assert.strictEqual(err.path, nonexistentFile);
    assert.strictEqual(err.filename, nonexistentFile);
    assert.strictEqual(
      err.message,
      `ENOENT: no such file or directory, watch '${nonexistentFile}'`);
    assert.strictEqual(err.errno, uv.UV_ENOENT);
    assert.strictEqual(err.code, 'ENOENT');
    assert.strictEqual(err.syscall, 'watch');
    fs.unlinkSync(file);
    return true;
  };

  watcher.on('error', common.mustCall(validateError));

  // Simulate the invocation from the binding
  watcher._handle.onchange(uv.UV_ENOENT, 'ENOENT', nonexistentFile);
}
