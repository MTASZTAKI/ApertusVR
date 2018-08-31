// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

'use strict';
const common = require('../common');
const path = require('path');
const fs = require('fs');

const tmpdir = require('tmpdir');

const testDir = tmpdir.path;
const testsubdir = path.join(testDir, 'testsubdir');
const filepath = path.join(testsubdir, 'watch.txt');

function cleanup() {
  try { fs.unlinkSync(filepath); } catch (e) { }
  try { fs.rmdirSync(testsubdir); } catch (e) { }
}
process.on('exit', cleanup);
cleanup();

try { fs.mkdirSync(testsubdir, 0o700); } catch (e) {}

// Need a grace period, else the mkdirSync() above fires off an event.
setTimeout(function() {
  const watcher = fs.watch(testDir, { persistent: true }, common.mustNotCall());
  setTimeout(function() {
    fs.writeFileSync(filepath, 'test');
  }, 100);
  setTimeout(function() {
    watcher.close();
  }, 500);
}, 50);
