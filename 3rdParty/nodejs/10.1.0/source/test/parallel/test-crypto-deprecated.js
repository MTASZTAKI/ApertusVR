'use strict';
const common = require('../common');
if (!common.hasCrypto)
  common.skip('missing crypto');

const assert = require('assert');
const crypto = require('crypto');
const tls = require('tls');

common.expectWarning('DeprecationWarning', [
  ['crypto.Credentials is deprecated. Use tls.SecureContext instead.',
   'DEP0011'],
  ['crypto.createCredentials is deprecated. Use tls.createSecureContext ' +
  'instead.', 'DEP0010'],
  ['crypto.Decipher.finaltol is deprecated. Use crypto.Decipher.final instead.',
   'DEP0105']
]);

// Accessing the deprecated function is enough to trigger the warning event.
// It does not need to be called. So the assert serves the purpose of both
// triggering the warning event and confirming that the deprecated function is
// mapped to the correct non-deprecated function.
assert.strictEqual(crypto.Credentials, tls.SecureContext);
assert.strictEqual(crypto.createCredentials, tls.createSecureContext);

{
  const cipher = crypto.createCipheriv('aes-128-cbc', '0000000000000000',
                                       '0000000000000000');
  const ciphertext = cipher.update('Node.js', 'utf8', 'hex') +
                     cipher.final('hex');
  const decipher = crypto.createDecipheriv('aes-128-cbc', '0000000000000000',
                                           '0000000000000000');
  const plaintext = decipher.update(ciphertext, 'hex', 'utf8') +
                    decipher.finaltol('utf8');
  assert.strictEqual(plaintext, 'Node.js');
}
