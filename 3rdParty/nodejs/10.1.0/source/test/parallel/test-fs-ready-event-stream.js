'use strict';
const common = require('../common');
const fs = require('fs');
const path = require('path');
const tmpdir = require('../common/tmpdir');

const readStream = fs.createReadStream(__filename);
readStream.on('ready', common.mustCall(() => {}, 1));

const writeFile = path.join(tmpdir.path, 'write-fsreadyevent.txt');
tmpdir.refresh();
const writeStream = fs.createWriteStream(writeFile, { autoClose: true });
writeStream.on('ready', common.mustCall(() => {}, 1));
