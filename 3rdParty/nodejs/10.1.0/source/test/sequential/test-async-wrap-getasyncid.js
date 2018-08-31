'use strict';

const common = require('../common');
const assert = require('assert');
const fs = require('fs');
const fsPromises = fs.promises;
const net = require('net');
const providers = Object.assign({}, process.binding('async_wrap').Providers);
const fixtures = require('../common/fixtures');
const tmpdir = require('../common/tmpdir');
const { getSystemErrorName } = require('util');

common.crashOnUnhandledRejection();

// Make sure that all Providers are tested.
{
  const hooks = require('async_hooks').createHook({
    init(id, type) {
      if (type === 'NONE')
        throw new Error('received a provider type of NONE');
      delete providers[type];
    },
  }).enable();
  process.on('beforeExit', common.mustCall(() => {
    process.removeAllListeners('uncaughtException');
    hooks.disable();
    delete providers.NONE;  // Should never be used.

    // See test/pseudo-tty/test-async-wrap-getasyncid-tty.js
    // Requires an 'actual' tty fd to be available.
    delete providers.TTYWRAP;

    // TODO(jasnell): Test for these
    delete providers.HTTP2SESSION;
    delete providers.HTTP2STREAM;
    delete providers.HTTP2PING;
    delete providers.HTTP2SETTINGS;
    delete providers.STREAMPIPE;

    const objKeys = Object.keys(providers);
    if (objKeys.length > 0)
      process._rawDebug(objKeys);
    assert.strictEqual(objKeys.length, 0);
  }));
}

function testUninitialized(req, ctor_name) {
  assert.strictEqual(typeof req.getAsyncId, 'function');
  assert.strictEqual(req.getAsyncId(), -1);
  assert.strictEqual(req.constructor.name, ctor_name);
}

function testInitialized(req, ctor_name) {
  assert.strictEqual(typeof req.getAsyncId, 'function');
  assert(Number.isSafeInteger(req.getAsyncId()));
  assert(req.getAsyncId() > 0);
  assert.strictEqual(req.constructor.name, ctor_name);
}


{
  const cares = process.binding('cares_wrap');
  const dns = require('dns');

  testUninitialized(new cares.GetAddrInfoReqWrap(), 'GetAddrInfoReqWrap');
  testUninitialized(new cares.GetNameInfoReqWrap(), 'GetNameInfoReqWrap');
  testUninitialized(new cares.QueryReqWrap(), 'QueryReqWrap');

  testInitialized(dns.lookup('www.google.com', () => {}), 'GetAddrInfoReqWrap');
  testInitialized(dns.lookupService('::1', 22, () => {}), 'GetNameInfoReqWrap');

  const resolver = new dns.Resolver();
  resolver.setServers(['127.0.0.1']);
  testInitialized(resolver._handle, 'ChannelWrap');
  testInitialized(resolver.resolve6('::1', () => {}), 'QueryReqWrap');
  resolver.cancel();
}


{
  const FSEvent = process.binding('fs_event_wrap').FSEvent;
  testInitialized(new FSEvent(), 'FSEvent');
}


{
  const JSStream = process.binding('js_stream').JSStream;
  testInitialized(new JSStream(), 'JSStream');
}


{
  // We don't want to expose getAsyncId for promises but we need to construct
  // one so that the corresponding provider type is removed from the
  // providers list.
  new Promise((res) => res(5));
}


if (common.hasCrypto) { // eslint-disable-line node-core/crypto-check
  const crypto = require('crypto');

  // The handle for PBKDF2 and RandomBytes isn't returned by the function call,
  // so need to check it from the callback.

  const mc = common.mustCall(function pb() {
    testInitialized(this, 'PBKDF2');
  });
  crypto.pbkdf2('password', 'salt', 1, 20, 'sha256', mc);

  crypto.randomBytes(1, common.mustCall(function rb() {
    testInitialized(this, 'RandomBytes');
  }));
}


{
  const binding = process.binding('fs');
  const path = require('path');

  const FSReqWrap = binding.FSReqWrap;
  const req = new FSReqWrap();
  req.oncomplete = () => { };

  testInitialized(req, 'FSReqWrap');
  binding.access(path.toNamespacedPath('../'), fs.F_OK, req);

  const StatWatcher = binding.StatWatcher;
  testInitialized(new StatWatcher(), 'StatWatcher');
}


{
  const HTTPParser = process.binding('http_parser').HTTPParser;
  testInitialized(new HTTPParser(), 'HTTPParser');
}


{
  const Gzip = require('zlib').Gzip;
  testInitialized(new Gzip()._handle, 'Zlib');
}

{
  const binding = process.binding('pipe_wrap');
  const handle = new binding.Pipe(binding.constants.IPC);
  testInitialized(handle, 'Pipe');
}

{
  tmpdir.refresh();

  const server = net.createServer(common.mustCall((socket) => {
    server.close();
  })).listen(common.PIPE, common.mustCall(() => {
    const binding = process.binding('pipe_wrap');
    const handle = new binding.Pipe(binding.constants.SOCKET);
    testInitialized(handle, 'Pipe');
    const req = new binding.PipeConnectWrap();
    testUninitialized(req, 'PipeConnectWrap');
    req.address = common.PIPE;
    req.oncomplete = common.mustCall(() => handle.close());
    handle.connect(req, req.address, req.oncomplete);
    testInitialized(req, 'PipeConnectWrap');
  }));
}

{
  const Process = process.binding('process_wrap').Process;
  testInitialized(new Process(), 'Process');
}

{
  const Signal = process.binding('signal_wrap').Signal;
  testInitialized(new Signal(), 'Signal');
}

{
  async function openTest() {
    const fd = await fsPromises.open(__filename, 'r');
    testInitialized(fd, 'FileHandle');
    await fd.close();
  }
  openTest().then(common.mustCall()).catch(common.mustNotCall());
}

{
  const binding = process.binding('stream_wrap');
  testUninitialized(new binding.WriteWrap(), 'WriteWrap');
}

{
  const stream_wrap = process.binding('stream_wrap');
  const tcp_wrap = process.binding('tcp_wrap');
  const server = net.createServer(common.mustCall((socket) => {
    server.close();
    socket.on('data', () => {
      socket.end();
      socket.destroy();
    });
    socket.resume();
  })).listen(0, common.localhostIPv4, common.mustCall(() => {
    const handle = new tcp_wrap.TCP(tcp_wrap.constants.SOCKET);
    const req = new tcp_wrap.TCPConnectWrap();
    const sreq = new stream_wrap.ShutdownWrap();
    testInitialized(handle, 'TCP');
    testUninitialized(req, 'TCPConnectWrap');
    testUninitialized(sreq, 'ShutdownWrap');

    sreq.oncomplete = common.mustCall(() => {
      handle.close();
    });

    req.oncomplete = common.mustCall(writeData);
    function writeData() {
      const wreq = new stream_wrap.WriteWrap();
      wreq.handle = handle;
      wreq.oncomplete = () => {
        handle.shutdown(sreq);
        testInitialized(sreq, 'ShutdownWrap');
      };
      const err = handle.writeLatin1String(wreq, 'hi'.repeat(100000));
      if (err)
        throw new Error(`write failed: ${getSystemErrorName(err)}`);
      if (!wreq.async) {
        testUninitialized(wreq, 'WriteWrap');
        // Synchronous finish. Write more data until we hit an
        // asynchronous write.
        return writeData();
      }
      testInitialized(wreq, 'WriteWrap');
    }
    req.address = common.localhostIPv4;
    req.port = server.address().port;
    const err = handle.connect(req, req.address, req.port);
    assert.strictEqual(err, 0);
    testInitialized(req, 'TCPConnectWrap');
  }));
}


{
  const TimerWrap = process.binding('timer_wrap').Timer;
  testInitialized(new TimerWrap(), 'Timer');
}


if (common.hasCrypto) { // eslint-disable-line node-core/crypto-check
  const { TCP, constants: TCPConstants } = process.binding('tcp_wrap');
  const tcp = new TCP(TCPConstants.SOCKET);

  const ca = fixtures.readSync('test_ca.pem', 'ascii');
  const cert = fixtures.readSync('test_cert.pem', 'ascii');
  const key = fixtures.readSync('test_key.pem', 'ascii');

  const credentials = require('tls').createSecureContext({ ca, cert, key });

  // TLSWrap is exposed, but needs to be instantiated via tls_wrap.wrap().
  const tls_wrap = process.binding('tls_wrap');
  testInitialized(
    tls_wrap.wrap(tcp._externalStream, credentials.context, true), 'TLSWrap');
}

{
  const binding = process.binding('udp_wrap');
  const handle = new binding.UDP();
  const req = new binding.SendWrap();
  testInitialized(handle, 'UDP');
  testUninitialized(req, 'SendWrap');

  handle.bind('0.0.0.0', 0, undefined);
  const addr = {};
  handle.getsockname(addr);
  req.address = '127.0.0.1';
  req.port = addr.port;
  req.oncomplete = () => handle.close();
  handle.send(req, [Buffer.alloc(1)], 1, req.port, req.address, true);
  testInitialized(req, 'SendWrap');
}

if (process.config.variables.v8_enable_inspector !== 0) {
  const binding = process.binding('inspector');
  const handle = new binding.Connection(() => {});
  testInitialized(handle, 'Connection');
  handle.disconnect();
}
