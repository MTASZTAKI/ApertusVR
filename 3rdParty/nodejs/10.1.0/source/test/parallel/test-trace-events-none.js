'use strict';
const common = require('../common');
const assert = require('assert');
const cp = require('child_process');

const CODE =
  'setTimeout(() => { for (var i = 0; i < 100000; i++) { "test" + i } }, 1)';
const FILE_NAME = 'node_trace.1.log';

const tmpdir = require('../common/tmpdir');
tmpdir.refresh();
process.chdir(tmpdir.path);

const proc_no_categories = cp.spawn(
  process.execPath,
  [ '--trace-event-categories', '""', '-e', CODE ]
);

proc_no_categories.once('exit', common.mustCall(() => {
  assert(!common.fileExists(FILE_NAME));
}));
