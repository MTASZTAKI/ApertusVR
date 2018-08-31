# Deprecated APIs

<!--introduced_in=v7.7.0-->
<!-- type=misc -->

Node.js may deprecate APIs when either: (a) use of the API is considered to be
unsafe, (b) an improved alternative API has been made available, or (c)
breaking changes to the API are expected in a future major release.

Node.js utilizes three kinds of Deprecations:

* Documentation-only
* Runtime
* End-of-Life

A Documentation-only deprecation is one that is expressed only within the
Node.js API docs. These generate no side-effects while running Node.js.
Some Documentation-only deprecations trigger a runtime warning when launched
with [`--pending-deprecation`][] flag (or its alternative,
`NODE_PENDING_DEPRECATION=1` environment variable), similarly to Runtime
deprecations below. Documentation-only deprecations that support that flag
are explicitly labeled as such in the
[list of Deprecated APIs](#deprecations_list_of_deprecated_apis).

A Runtime deprecation will, by default, generate a process warning that will
be printed to `stderr` the first time the deprecated API is used. When the
`--throw-deprecation` command-line flag is used, a Runtime deprecation will
cause an error to be thrown.

An End-of-Life deprecation is used to identify code that either has been
removed or will soon be removed from Node.js.

## Revoking deprecations

Occasionally, the deprecation of an API may be reversed. In such situations,
this document will be updated with information relevant to the decision.
However, the deprecation identifier will not be modified.

## List of Deprecated APIs

<a id="DEP0001"></a>
### DEP0001: http.OutgoingMessage.prototype.flush

Type: Runtime

The `OutgoingMessage.prototype.flush()` method is deprecated. Use
`OutgoingMessage.prototype.flushHeaders()` instead.

<a id="DEP0002"></a>
### DEP0002: require('\_linklist')

Type: End-of-Life

The `_linklist` module is deprecated. Please use a userland alternative.

<a id="DEP0003"></a>
### DEP0003: \_writableState.buffer

Type: Runtime

The `_writableState.buffer` property is deprecated. Use the
`_writableState.getBuffer()` method instead.

<a id="DEP0004"></a>
### DEP0004: CryptoStream.prototype.readyState

Type: Documentation-only

The `CryptoStream.prototype.readyState` property is deprecated and should not
be used.

<a id="DEP0005"></a>
### DEP0005: Buffer() constructor

Type: Runtime (supports [`--pending-deprecation`][])

The `Buffer()` function and `new Buffer()` constructor are deprecated due to
API usability issues that can potentially lead to accidental security issues.

As an alternative, use of the following methods of constructing `Buffer` objects
is strongly recommended:

* [`Buffer.alloc(size[, fill[, encoding]])`][alloc] - Create a `Buffer` with
  *initialized* memory.
* [`Buffer.allocUnsafe(size)`][alloc_unsafe_size] - Create a `Buffer` with
  *uninitialized* memory.
* [`Buffer.allocUnsafeSlow(size)`][] - Create a `Buffer` with *uninitialized*
   memory.
* [`Buffer.from(array)`][] - Create a `Buffer` with a copy of `array`
* [`Buffer.from(arrayBuffer[, byteOffset[, length]])`][from_arraybuffer] -
  Create a `Buffer` that wraps the given `arrayBuffer`.
* [`Buffer.from(buffer)`][] - Create a `Buffer` that copies `buffer`.
* [`Buffer.from(string[, encoding])`][from_string_encoding] - Create a `Buffer`
  that copies `string`.

As of v10.0.0, a deprecation warning is printed at runtime when
`--pending-deprecation` is used or when the calling code is
outside `node_modules` in order to better target developers, rather than users.

<a id="DEP0006"></a>
### DEP0006: child\_process options.customFds

Type: Runtime

Within the [`child_process`][] module's `spawn()`, `fork()`, and `exec()`
methods, the `options.customFds` option is deprecated. The `options.stdio`
option should be used instead.

<a id="DEP0007"></a>
### DEP0007: Replace cluster worker.suicide with worker.exitedAfterDisconnect

Type: End-of-Life

In an earlier version of the Node.js `cluster`, a boolean property with the name
`suicide` was added to the `Worker` object. The intent of this property was to
provide an indication of how and why the `Worker` instance exited. In Node.js
6.0.0, the old property was deprecated and replaced with a new
[`worker.exitedAfterDisconnect`][] property. The old property name did not
precisely describe the actual semantics and was unnecessarily emotion-laden.

<a id="DEP0008"></a>
### DEP0008: require('constants')

Type: Documentation-only

The `constants` module has been deprecated. When requiring access to constants
relevant to specific Node.js builtin modules, developers should instead refer
to the `constants` property exposed by the relevant module. For instance,
`require('fs').constants` and `require('os').constants`.

<a id="DEP0009"></a>
### DEP0009: crypto.pbkdf2 without digest

Type: End-of-Life

Use of the [`crypto.pbkdf2()`][] API without specifying a digest was deprecated
in Node.js 6.0 because the method defaulted to using the non-recommended
`'SHA1'` digest. Previously, a deprecation warning was printed. Starting in
Node.js 8.0.0, calling `crypto.pbkdf2()` or `crypto.pbkdf2Sync()` with an
undefined `digest` will throw a `TypeError`.

<a id="DEP0010"></a>
### DEP0010: crypto.createCredentials

Type: Runtime

The [`crypto.createCredentials()`][] API is deprecated. Please use
[`tls.createSecureContext()`][] instead.

<a id="DEP0011"></a>
### DEP0011: crypto.Credentials

Type: Runtime

The `crypto.Credentials` class is deprecated. Please use [`tls.SecureContext`][]
instead.

<a id="DEP0012"></a>
### DEP0012: Domain.dispose

Type: End-of-Life

`Domain.dispose()` has been removed. Recover from failed I/O actions
explicitly via error event handlers set on the domain instead.

<a id="DEP0013"></a>
### DEP0013: fs asynchronous function without callback

Type: End-of-Life

Calling an asynchronous function without a callback throws a `TypeError`
in Node.js 10.0.0 onwards. (See https://github.com/nodejs/node/pull/12562.)

<a id="DEP0014"></a>
### DEP0014: fs.read legacy String interface

Type: End-of-Life

The [`fs.read()`][] legacy `String` interface is deprecated. Use the `Buffer`
API as mentioned in the documentation instead.

<a id="DEP0015"></a>
### DEP0015: fs.readSync legacy String interface

Type: End-of-Life

The [`fs.readSync()`][] legacy `String` interface is deprecated. Use the
`Buffer` API as mentioned in the documentation instead.

<a id="DEP0016"></a>
### DEP0016: GLOBAL/root

Type: Runtime

The `GLOBAL` and `root` aliases for the `global` property have been deprecated
and should no longer be used.

<a id="DEP0017"></a>
### DEP0017: Intl.v8BreakIterator

Type: End-of-Life

`Intl.v8BreakIterator` was a non-standard extension and has been removed.
See [`Intl.Segmenter`](https://github.com/tc39/proposal-intl-segmenter).

<a id="DEP0018"></a>
### DEP0018: Unhandled promise rejections

Type: Runtime

Unhandled promise rejections are deprecated. In the future, promise rejections
that are not handled will terminate the Node.js process with a non-zero exit
code.

<a id="DEP0019"></a>
### DEP0019: require('.') resolved outside directory

Type: Runtime

In certain cases, `require('.')` may resolve outside the package directory.
This behavior is deprecated and will be removed in a future major Node.js
release.

<a id="DEP0020"></a>
### DEP0020: Server.connections

Type: Runtime

The [`Server.connections`][] property is deprecated. Please use the
[`Server.getConnections()`][] method instead.

<a id="DEP0021"></a>
### DEP0021: Server.listenFD

Type: Runtime

The `Server.listenFD()` method is deprecated. Please use
[`Server.listen({fd: <number>})`][] instead.

<a id="DEP0022"></a>
### DEP0022: os.tmpDir()

Type: Runtime

The `os.tmpDir()` API is deprecated. Please use [`os.tmpdir()`][] instead.

<a id="DEP0023"></a>
### DEP0023: os.getNetworkInterfaces()

Type: Runtime

The `os.getNetworkInterfaces()` method is deprecated. Please use the
[`os.networkInterfaces`][] property instead.

<a id="DEP0024"></a>
### DEP0024: REPLServer.prototype.convertToContext()

Type: End-of-Life

The `REPLServer.prototype.convertToContext()` API is deprecated and should
not be used.

<a id="DEP0025"></a>
### DEP0025: require('sys')

Type: Runtime

The `sys` module is deprecated. Please use the [`util`][] module instead.

<a id="DEP0026"></a>
### DEP0026: util.print()

Type: Runtime

The [`util.print()`][] API is deprecated. Please use [`console.log()`][]
instead.

<a id="DEP0027"></a>
### DEP0027: util.puts()

Type: Runtime

The [`util.puts()`][] API is deprecated. Please use [`console.log()`][] instead.

<a id="DEP0028"></a>
### DEP0028: util.debug()

Type: Runtime

The [`util.debug()`][] API is deprecated. Please use [`console.error()`][]
instead.

<a id="DEP0029"></a>
### DEP0029: util.error()

Type: Runtime

The [`util.error()`][] API is deprecated. Please use [`console.error()`][]
instead.

<a id="DEP0030"></a>
### DEP0030: SlowBuffer

Type: Documentation-only

The [`SlowBuffer`][] class has been deprecated. Please use
[`Buffer.allocUnsafeSlow(size)`][] instead.

<a id="DEP0031"></a>
### DEP0031: ecdh.setPublicKey()

Type: Documentation-only

The [`ecdh.setPublicKey()`][] method is now deprecated as its inclusion in the
API is not useful.

<a id="DEP0032"></a>
### DEP0032: domain module

Type: Documentation-only

The [`domain`][] module is deprecated and should not be used.

<a id="DEP0033"></a>
### DEP0033: EventEmitter.listenerCount()

Type: Documentation-only

The [`EventEmitter.listenerCount(emitter, eventName)`][] API has been
deprecated. Please use [`emitter.listenerCount(eventName)`][] instead.

<a id="DEP0034"></a>
### DEP0034: fs.exists(path, callback)

Type: Documentation-only

The [`fs.exists(path, callback)`][] API has been deprecated. Please use
[`fs.stat()`][] or [`fs.access()`][] instead.

<a id="DEP0035"></a>
### DEP0035: fs.lchmod(path, mode, callback)

Type: Documentation-only

The [`fs.lchmod(path, mode, callback)`][] API has been deprecated.

<a id="DEP0036"></a>
### DEP0036: fs.lchmodSync(path, mode)

Type: Documentation-only

The [`fs.lchmodSync(path, mode)`][] API has been deprecated.

<a id="DEP0037"></a>
### DEP0037: fs.lchown(path, uid, gid, callback)

Type: Documentation-only

The [`fs.lchown(path, uid, gid, callback)`][] API has been deprecated.

<a id="DEP0038"></a>
### DEP0038: fs.lchownSync(path, uid, gid)

Type: Documentation-only

The [`fs.lchownSync(path, uid, gid)`][] API has been deprecated.

<a id="DEP0039"></a>
### DEP0039: require.extensions

Type: Documentation-only

The [`require.extensions`][] property has been deprecated.

<a id="DEP0040"></a>
### DEP0040: punycode module

Type: Documentation-only

The [`punycode`][] module has been deprecated. Please use a userland alternative
instead.

<a id="DEP0041"></a>
### DEP0041: NODE\_REPL\_HISTORY\_FILE environment variable

Type: End-of-Life

The `NODE_REPL_HISTORY_FILE` environment variable was removed. Please use
`NODE_REPL_HISTORY` instead.

<a id="DEP0042"></a>
### DEP0042: tls.CryptoStream

Type: Documentation-only

The [`tls.CryptoStream`][] class has been deprecated. Please use
[`tls.TLSSocket`][] instead.

<a id="DEP0043"></a>
### DEP0043: tls.SecurePair

Type: Documentation-only

The [`tls.SecurePair`][] class has been deprecated. Please use
[`tls.TLSSocket`][] instead.

<a id="DEP0044"></a>
### DEP0044: util.isArray()

Type: Documentation-only

The [`util.isArray()`][] API has been deprecated. Please use `Array.isArray()`
instead.

<a id="DEP0045"></a>
### DEP0045: util.isBoolean()

Type: Documentation-only

The [`util.isBoolean()`][] API has been deprecated.

<a id="DEP0046"></a>
### DEP0046: util.isBuffer()

Type: Documentation-only

The [`util.isBuffer()`][] API has been deprecated. Please use
[`Buffer.isBuffer()`][] instead.

<a id="DEP0047"></a>
### DEP0047: util.isDate()

Type: Documentation-only

The [`util.isDate()`][] API has been deprecated.

<a id="DEP0048"></a>
### DEP0048: util.isError()

Type: Documentation-only

The [`util.isError()`][] API has been deprecated.

<a id="DEP0049"></a>
### DEP0049: util.isFunction()

Type: Documentation-only

The [`util.isFunction()`][] API has been deprecated.

<a id="DEP0050"></a>
### DEP0050: util.isNull()

Type: Documentation-only

The [`util.isNull()`][] API has been deprecated.

<a id="DEP0051"></a>
### DEP0051: util.isNullOrUndefined()

Type: Documentation-only

The [`util.isNullOrUndefined()`][] API has been deprecated.

<a id="DEP0052"></a>
### DEP0052: util.isNumber()

Type: Documentation-only

The [`util.isNumber()`][] API has been deprecated.

<a id="DEP0053"></a>
### DEP0053 util.isObject()

Type: Documentation-only

The [`util.isObject()`][] API has been deprecated.

<a id="DEP0054"></a>
### DEP0054: util.isPrimitive()

Type: Documentation-only

The [`util.isPrimitive()`][] API has been deprecated.

<a id="DEP0055"></a>
### DEP0055: util.isRegExp()

Type: Documentation-only

The [`util.isRegExp()`][] API has been deprecated.

<a id="DEP0056"></a>
### DEP0056: util.isString()

Type: Documentation-only

The [`util.isString()`][] API has been deprecated.

<a id="DEP0057"></a>
### DEP0057: util.isSymbol()

Type: Documentation-only

The [`util.isSymbol()`][] API has been deprecated.

<a id="DEP0058"></a>
### DEP0058: util.isUndefined()

Type: Documentation-only

The [`util.isUndefined()`][] API has been deprecated.

<a id="DEP0059"></a>
### DEP0059: util.log()

Type: Documentation-only

The [`util.log()`][] API has been deprecated.

<a id="DEP0060"></a>
### DEP0060: util.\_extend()

Type: Documentation-only

The [`util._extend()`][] API has been deprecated.

<a id="DEP0061"></a>
### DEP0061: fs.SyncWriteStream

Type: Runtime

The `fs.SyncWriteStream` class was never intended to be a publicly accessible
API. No alternative API is available. Please use a userland alternative.

<a id="DEP0062"></a>
### DEP0062: node --debug

Type: Runtime

`--debug` activates the legacy V8 debugger interface, which has been removed as
of V8 5.8. It is replaced by Inspector which is activated with `--inspect`
instead.

<a id="DEP0063"></a>
### DEP0063: ServerResponse.prototype.writeHeader()

Type: Documentation-only

The `http` module `ServerResponse.prototype.writeHeader()` API has been
deprecated. Please use `ServerResponse.prototype.writeHead()` instead.

The `ServerResponse.prototype.writeHeader()` method was never documented as an
officially supported API.

<a id="DEP0064"></a>
### DEP0064: tls.createSecurePair()

Type: Runtime

The `tls.createSecurePair()` API was deprecated in documentation in Node.js
0.11.3. Users should use `tls.Socket` instead.

<a id="DEP0065"></a>
### DEP0065: repl.REPL_MODE_MAGIC and NODE_REPL_MODE=magic

Type: End-of-Life

The `repl` module's `REPL_MODE_MAGIC` constant, used for `replMode` option, has
been removed. Its behavior has been functionally identical to that of
`REPL_MODE_SLOPPY` since Node.js 6.0.0, when V8 5.0 was imported. Please use
`REPL_MODE_SLOPPY` instead.

The `NODE_REPL_MODE` environment variable is used to set the underlying
`replMode` of an interactive `node` session. Its value, `magic`, is also
removed. Please use `sloppy` instead.

<a id="DEP0066"></a>
### DEP0066: outgoingMessage.\_headers, outgoingMessage.\_headerNames

Type: Documentation-only

The `http` module `outgoingMessage._headers` and `outgoingMessage._headerNames`
properties have been deprecated. Please instead use one of the public methods
(e.g. `outgoingMessage.getHeader()`, `outgoingMessage.getHeaders()`,
`outgoingMessage.getHeaderNames()`, `outgoingMessage.hasHeader()`,
`outgoingMessage.removeHeader()`, `outgoingMessage.setHeader()`) for working
with outgoing headers.

The `outgoingMessage._headers` and `outgoingMessage._headerNames` properties
were never documented as officially supported properties.

<a id="DEP0067"></a>
### DEP0067: OutgoingMessage.prototype.\_renderHeaders

Type: Documentation-only

The `http` module `OutgoingMessage.prototype._renderHeaders()` API has been
deprecated.

The `OutgoingMessage.prototype._renderHeaders` property was never documented as
an officially supported API.

<a id="DEP0068"></a>
### DEP0068: node debug

Type: Runtime

`node debug` corresponds to the legacy CLI debugger which has been replaced with
a V8-inspector based CLI debugger available through `node inspect`.

<a id="DEP0069"></a>
### DEP0069: vm.runInDebugContext(string)

Type: End-of-Life

DebugContext has been removed in V8 and is not available in Node.js 10+.

DebugContext was an experimental API.

<a id="DEP0070"></a>
### DEP0070: async_hooks.currentId()

Type: End-of-Life

`async_hooks.currentId()` was renamed to `async_hooks.executionAsyncId()` for
clarity.

This change was made while `async_hooks` was an experimental API.

<a id="DEP0071"></a>
### DEP0071: async_hooks.triggerId()

Type: End-of-Life

`async_hooks.triggerId()` was renamed to `async_hooks.triggerAsyncId()` for
clarity.

This change was made while `async_hooks` was an experimental API.

<a id="DEP0072"></a>
### DEP0072: async_hooks.AsyncResource.triggerId()

Type: End-of-Life

`async_hooks.AsyncResource.triggerId()` was renamed to
`async_hooks.AsyncResource.triggerAsyncId()` for clarity.

This change was made while `async_hooks` was an experimental API.

<a id="DEP0073"></a>
### DEP0073: Several internal properties of net.Server

Type: End-of-Life

Accessing several internal, undocumented properties of `net.Server` instances
with inappropriate names has been deprecated.

As the original API was undocumented and not generally useful for non-internal
code, no replacement API is provided.

<a id="DEP0074"></a>
### DEP0074: REPLServer.bufferedCommand

Type: Runtime

The `REPLServer.bufferedCommand` property was deprecated in favor of
[`REPLServer.clearBufferedCommand()`][].

<a id="DEP0075"></a>
### DEP0075: REPLServer.parseREPLKeyword()

Type: Runtime

`REPLServer.parseREPLKeyword()` was removed from userland visibility.

<a id="DEP0076"></a>
### DEP0076: tls.parseCertString()

Type: Runtime

`tls.parseCertString()` is a trivial parsing helper that was made public by
mistake. This function can usually be replaced with:

```js
const querystring = require('querystring');
querystring.parse(str, '\n', '=');
```

This function is not completely equivalent to `querystring.parse()`. One
difference is that `querystring.parse()` does url decoding:

```sh
> querystring.parse('%E5%A5%BD=1', '\n', '=');
{ '好': '1' }
> tls.parseCertString('%E5%A5%BD=1');
{ '%E5%A5%BD': '1' }
```

<a id="DEP0077"></a>
### DEP0077: Module.\_debug()

Type: Runtime

`Module._debug()` has been deprecated.

The `Module._debug()` function was never documented as an officially
supported API.

<a id="DEP0078"></a>
### DEP0078: REPLServer.turnOffEditorMode()

Type: Runtime

`REPLServer.turnOffEditorMode()` was removed from userland visibility.

<a id="DEP0079"></a>
### DEP0079: Custom inspection function on Objects via .inspect()

Type: Runtime

Using a property named `inspect` on an object to specify a custom inspection
function for [`util.inspect()`][] is deprecated. Use [`util.inspect.custom`][]
instead. For backward compatibility with Node.js prior to version 6.4.0, both
may be specified.

<a id="DEP0080"></a>
### DEP0080: path.\_makeLong()

Type: Documentation-only

The internal `path._makeLong()` was not intended for public use. However,
userland modules have found it useful. The internal API has been deprecated
and replaced with an identical, public `path.toNamespacedPath()` method.

<a id="DEP0081"></a>
### DEP0081: fs.truncate() using a file descriptor

Type: Runtime

`fs.truncate()` `fs.truncateSync()` usage with a file descriptor has been
deprecated. Please use `fs.ftruncate()` or `fs.ftruncateSync()` to work with
file descriptors.

<a id="DEP0082"></a>
### DEP0082: REPLServer.prototype.memory()

Type: Runtime

`REPLServer.prototype.memory()` is only necessary for the internal mechanics of
the `REPLServer` itself. Do not use this function.

<a id="DEP0083"></a>
### DEP0083: Disabling ECDH by setting ecdhCurve to false

Type: Runtime

The `ecdhCurve` option to `tls.createSecureContext()` and `tls.TLSSocket` could
be set to `false` to disable ECDH entirely on the server only. This mode is
deprecated in preparation for migrating to OpenSSL 1.1.0 and consistency with
the client. Use the `ciphers` parameter instead.

<a id="DEP0084"></a>
### DEP0084: requiring bundled internal dependencies

Type: Runtime

Since Node.js versions 4.4.0 and 5.2.0, several modules only intended for
internal usage are mistakenly exposed to user code through `require()`. These
modules are:

- `v8/tools/codemap`
- `v8/tools/consarray`
- `v8/tools/csvparser`
- `v8/tools/logreader`
- `v8/tools/profile_view`
- `v8/tools/profile`
- `v8/tools/SourceMap`
- `v8/tools/splaytree`
- `v8/tools/tickprocessor-driver`
- `v8/tools/tickprocessor`
- `node-inspect/lib/_inspect` (from 7.6.0)
- `node-inspect/lib/internal/inspect_client` (from 7.6.0)
- `node-inspect/lib/internal/inspect_repl` (from 7.6.0)

The `v8/*` modules do not have any exports, and if not imported in a specific
order would in fact throw errors. As such there are virtually no legitimate use
cases for importing them through `require()`.

On the other hand, `node-inspect` may be installed locally through a package
manager, as it is published on the npm registry under the same name. No source
code modification is necessary if that is done.

<a id="DEP0085"></a>
### DEP0085: AsyncHooks Sensitive API

Type: End-of-Life

The AsyncHooks Sensitive API was never documented and had various minor issues.
(See https://github.com/nodejs/node/issues/15572.) Use the `AsyncResource`
API instead.

<a id="DEP0086"></a>
### DEP0086: Remove runInAsyncIdScope

Type: End-of-Life

`runInAsyncIdScope` doesn't emit the `'before'` or `'after'` event and can thus
cause a lot of issues. See https://github.com/nodejs/node/issues/14328 for more
details.

<a id="DEP0089"></a>
### DEP0089: require('assert')

Type: Documentation-only

Importing assert directly is not recommended as the exposed functions will use
loose equality checks. Use `require('assert').strict` instead. The API is the
same as the legacy assert but it will always use strict equality checks.

<a id="DEP0090"></a>
### DEP0090: Invalid GCM authentication tag lengths

Type: Runtime

Node.js supports all GCM authentication tag lengths which are accepted by
OpenSSL when calling [`decipher.setAuthTag()`][]. This behavior will change in
a future version at which point only authentication tag lengths of 128, 120,
112, 104, 96, 64, and 32 bits will be allowed. Authentication tags whose length
is not included in this list will be considered invalid in compliance with
[NIST SP 800-38D][].

<a id="DEP0091"></a>
### DEP0091: crypto.DEFAULT_ENCODING

Type: Runtime

The [`crypto.DEFAULT_ENCODING`][] property is deprecated.

<a id="DEP0092"></a>
### DEP0092: Top-level `this` bound to `module.exports`

Type: Documentation-only

Assigning properties to the top-level `this` as an alternative
to `module.exports` is deprecated. Developers should use `exports`
or `module.exports` instead.

<a id="DEP0093"></a>
### DEP0093: crypto.fips is deprecated and replaced.

Type: Documentation-only

The [`crypto.fips`][] property is deprecated. Please use `crypto.setFips()`
and `crypto.getFips()` instead.

<a id="DEP0094"></a>
### DEP0094: Using `assert.fail()` with more than one argument.

Type: Runtime

Using `assert.fail()` with more than one argument is deprecated. Use
`assert.fail()` with only one argument or use a different `assert` module
method.

<a id="DEP0095"></a>
### DEP0095: timers.enroll()

Type: Runtime

`timers.enroll()` is deprecated. Please use the publicly documented
[`setTimeout()`][] or [`setInterval()`][] instead.

<a id="DEP0096"></a>
### DEP0096: timers.unenroll()

Type: Runtime

`timers.unenroll()` is deprecated. Please use the publicly documented
[`clearTimeout()`][] or [`clearInterval()`][] instead.

<a id="DEP0097"></a>
### DEP0097: MakeCallback with domain property

Type: Runtime

Users of `MakeCallback` that add the `domain` property to carry context,
should start using the `async_context` variant of `MakeCallback` or
`CallbackScope`, or the high-level `AsyncResource` class.

<a id="DEP0098"></a>
### DEP0098: AsyncHooks Embedder AsyncResource.emitBefore and AsyncResource.emitAfter APIs

Type: Runtime

The embedded API provided by AsyncHooks exposes `.emitBefore()` and
`.emitAfter()` methods which are very easy to use incorrectly which can lead
to unrecoverable errors.

Use [`asyncResource.runInAsyncScope()`][] API instead which provides a much
safer, and more convenient, alternative. See
https://github.com/nodejs/node/pull/18513 for more details.

<a id="DEP0099"></a>
### DEP0099: async context-unaware node::MakeCallback C++ APIs

Type: Compile-time

Certain versions of `node::MakeCallback` APIs available to native modules are
deprecated. Please use the versions of the API that accept an `async_context`
parameter.

<a id="DEP0100"></a>
### DEP0100: process.assert()

Type: Runtime

`process.assert()` is deprecated. Please use the [`assert`][] module instead.

This was never a documented feature.

<a id="DEP0101"></a>
### DEP0101: --with-lttng

Type: End-of-Life

The `--with-lttng` compile-time option has been removed.

<a id="DEP0102"></a>
### DEP0102: Using `noAssert` in Buffer#(read|write) operations.

Type: End-of-Life

Using the `noAssert` argument has no functionality anymore. All input is going
to be verified, no matter if it is set to true or not. Skipping the verification
could lead to hard to find errors and crashes.

<a id="DEP0103"></a>
### DEP0103: process.binding('util').is[...] typechecks

Type: Documentation-only (supports [`--pending-deprecation`][])

Using `process.binding()` in general should be avoided. The type checking
methods in particular can be replaced by using [`util.types`][].

<a id="DEP0104"></a>
### DEP0104: process.env string coercion

Type: Documentation-only (supports [`--pending-deprecation`][])

When assigning a non-string property to [`process.env`][], the assigned value is
implicitly converted to a string. This behavior is deprecated if the assigned
value is not a string, boolean, or number. In the future, such assignment may
result in a thrown error. Please convert the property to a string before
assigning it to `process.env`.

<a id="DEP0105"></a>
### DEP0105: decipher.finaltol

Type: Runtime

`decipher.finaltol()` has never been documented and is currently an alias for
[`decipher.final()`][]. In the future, this API will likely be removed, and it
is recommended to use [`decipher.final()`][] instead.

<a id="DEP0106"></a>
### DEP0106: crypto.createCipher and crypto.createDecipher

Type: Documentation-only

Using [`crypto.createCipher()`][] and [`crypto.createDecipher()`][] should be
avoided as they use a weak key derivation function (MD5 with no salt) and static
initialization vectors. It is recommended to derive a key using
[`crypto.pbkdf2()`][] and to use [`crypto.createCipheriv()`][] and
[`crypto.createDecipheriv()`][] to obtain the [`Cipher`][] and [`Decipher`][]
objects respectively.

<a id="DEP0107"></a>
### DEP0107: tls.convertNPNProtocols()

Type: Runtime

This was an undocumented helper function not intended for use outside Node.js
core and obsoleted by the removal of NPN (Next Protocol Negotiation) support.

<a id="DEP0108"></a>
### DEP0108: zlib.bytesRead

Type: Documentation-only

Deprecated alias for [`zlib.bytesWritten`][]. This original name was chosen
because it also made sense to interpret the value as the number of bytes
read by the engine, but is inconsistent with other streams in Node.js that
expose values under these names.

[`--pending-deprecation`]: cli.html#cli_pending_deprecation
[`Buffer.allocUnsafeSlow(size)`]: buffer.html#buffer_class_method_buffer_allocunsafeslow_size
[`Buffer.from(array)`]: buffer.html#buffer_class_method_buffer_from_array
[`Buffer.from(buffer)`]: buffer.html#buffer_class_method_buffer_from_buffer
[`Buffer.isBuffer()`]: buffer.html#buffer_class_method_buffer_isbuffer_obj
[`Cipher`]: crypto.html#crypto_class_cipher
[`Decipher`]: crypto.html#crypto_class_decipher
[`assert`]: assert.html
[`clearInterval()`]: timers.html#timers_clearinterval_timeout
[`clearTimeout()`]: timers.html#timers_cleartimeout_timeout
[`EventEmitter.listenerCount(emitter, eventName)`]: events.html#events_eventemitter_listenercount_emitter_eventname
[`Server.connections`]: net.html#net_server_connections
[`Server.getConnections()`]: net.html#net_server_getconnections_callback
[`Server.listen({fd: <number>})`]: net.html#net_server_listen_handle_backlog_callback
[`SlowBuffer`]: buffer.html#buffer_class_slowbuffer
[`asyncResource.runInAsyncScope()`]: async_hooks.html#async_hooks_asyncresource_runinasyncscope_fn_thisarg_args
[`child_process`]: child_process.html
[`console.error()`]: console.html#console_console_error_data_args
[`console.log()`]: console.html#console_console_log_data_args
[`crypto.createCipher()`]: crypto.html#crypto_crypto_createcipher_algorithm_password_options
[`crypto.createCipheriv()`]: crypto.html#crypto_crypto_createcipheriv_algorithm_key_iv_options
[`crypto.createCredentials()`]: crypto.html#crypto_crypto_createcredentials_details
[`crypto.createDecipher()`]: crypto.html#crypto_crypto_createdecipher_algorithm_password_options
[`crypto.createDecipheriv()`]: crypto.html#crypto_crypto_createdecipheriv_algorithm_key_iv_options
[`crypto.DEFAULT_ENCODING`]: crypto.html#crypto_crypto_default_encoding
[`crypto.fips`]: crypto.html#crypto_crypto_fips
[`crypto.pbkdf2()`]: crypto.html#crypto_crypto_pbkdf2_password_salt_iterations_keylen_digest_callback
[`decipher.final()`]: crypto.html#crypto_decipher_final_outputencoding
[`decipher.setAuthTag()`]: crypto.html#crypto_decipher_setauthtag_buffer
[`domain`]: domain.html
[`ecdh.setPublicKey()`]: crypto.html#crypto_ecdh_setpublickey_publickey_encoding
[`emitter.listenerCount(eventName)`]: events.html#events_emitter_listenercount_eventname
[`fs.access()`]: fs.html#fs_fs_access_path_mode_callback
[`fs.exists(path, callback)`]: fs.html#fs_fs_exists_path_callback
[`fs.lchmod(path, mode, callback)`]: fs.html#fs_fs_lchmod_path_mode_callback
[`fs.lchmodSync(path, mode)`]: fs.html#fs_fs_lchmodsync_path_mode
[`fs.lchown(path, uid, gid, callback)`]: fs.html#fs_fs_lchown_path_uid_gid_callback
[`fs.lchownSync(path, uid, gid)`]: fs.html#fs_fs_lchownsync_path_uid_gid
[`fs.read()`]: fs.html#fs_fs_read_fd_buffer_offset_length_position_callback
[`fs.readSync()`]: fs.html#fs_fs_readsync_fd_buffer_offset_length_position
[`fs.stat()`]: fs.html#fs_fs_stat_path_callback
[`os.networkInterfaces`]: os.html#os_os_networkinterfaces
[`os.tmpdir()`]: os.html#os_os_tmpdir
[`process.env`]: process.html#process_process_env
[`punycode`]: punycode.html
[`require.extensions`]: modules.html#modules_require_extensions
[`setInterval()`]: timers.html#timers_setinterval_callback_delay_args
[`setTimeout()`]: timers.html#timers_settimeout_callback_delay_args
[`tls.CryptoStream`]: tls.html#tls_class_cryptostream
[`tls.SecureContext`]: tls.html#tls_tls_createsecurecontext_options
[`tls.SecurePair`]: tls.html#tls_class_securepair
[`tls.TLSSocket`]: tls.html#tls_class_tls_tlssocket
[`tls.createSecureContext()`]: tls.html#tls_tls_createsecurecontext_options
[`util._extend()`]: util.html#util_util_extend_target_source
[`util.debug()`]: util.html#util_util_debug_string
[`util.error()`]: util.html#util_util_error_strings
[`util.inspect()`]: util.html#util_util_inspect_object_options
[`util.inspect.custom`]: util.html#util_util_inspect_custom
[`util.isArray()`]: util.html#util_util_isarray_object
[`util.isBoolean()`]: util.html#util_util_isboolean_object
[`util.isBuffer()`]: util.html#util_util_isbuffer_object
[`util.isDate()`]: util.html#util_util_isdate_object
[`util.isError()`]: util.html#util_util_iserror_object
[`util.isFunction()`]: util.html#util_util_isfunction_object
[`util.isNull()`]: util.html#util_util_isnull_object
[`util.isNullOrUndefined()`]: util.html#util_util_isnullorundefined_object
[`util.isNumber()`]: util.html#util_util_isnumber_object
[`util.isObject()`]: util.html#util_util_isobject_object
[`util.isPrimitive()`]: util.html#util_util_isprimitive_object
[`util.isRegExp()`]: util.html#util_util_isregexp_object
[`util.isString()`]: util.html#util_util_isstring_object
[`util.isSymbol()`]: util.html#util_util_issymbol_object
[`util.isUndefined()`]: util.html#util_util_isundefined_object
[`util.log()`]: util.html#util_util_log_string
[`util.print()`]: util.html#util_util_print_strings
[`util.puts()`]: util.html#util_util_puts_strings
[`util.types`]: util.html#util_util_types
[`util`]: util.html
[`worker.exitedAfterDisconnect`]: cluster.html#cluster_worker_exitedafterdisconnect
[`zlib.bytesWritten`]: zlib.html#zlib_zlib_byteswritten
[alloc]: buffer.html#buffer_class_method_buffer_alloc_size_fill_encoding
[alloc_unsafe_size]: buffer.html#buffer_class_method_buffer_allocunsafe_size
[from_arraybuffer]: buffer.html#buffer_class_method_buffer_from_arraybuffer_byteoffset_length
[from_string_encoding]: buffer.html#buffer_class_method_buffer_from_string_encoding
[NIST SP 800-38D]: http://nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38d.pdf
[`REPLServer.clearBufferedCommand()`]: repl.html#repl_replserver_clearbufferedcommand
