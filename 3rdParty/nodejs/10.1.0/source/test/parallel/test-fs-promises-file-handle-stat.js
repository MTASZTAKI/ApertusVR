'use strict';

const common = require('../common');

// The following tests validate base functionality for the fs.promises
// FileHandle.stat method.

const { open } = require('fs').promises;
const path = require('path');
const tmpdir = require('../common/tmpdir');
const assert = require('assert');

tmpdir.refresh();
common.crashOnUnhandledRejection();

async function validateStat() {
  const filePath = path.resolve(tmpdir.path, 'tmp-read-file.txt');
  const fileHandle = await open(filePath, 'w+');
  const stats = await fileHandle.stat();
  assert.ok(stats.mtime instanceof Date);
}

validateStat()
  .then(common.mustCall());
