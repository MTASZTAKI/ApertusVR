'use strict';

const {
  ERR_INVALID_ARG_TYPE,
  ERR_NO_CRYPTO,
  ERR_UNKNOWN_SIGNAL
} = require('internal/errors').codes;
const { signals } = process.binding('constants').os;

const {
  createPromise,
  getHiddenValue,
  promiseResolve,
  promiseReject,
  setHiddenValue,
  arrow_message_private_symbol: kArrowMessagePrivateSymbolIndex,
  decorated_private_symbol: kDecoratedPrivateSymbolIndex
} = process.binding('util');
const { errmap } = process.binding('uv');

const noCrypto = !process.versions.openssl;

const experimentalWarnings = new Set();

const colorRegExp = /\u001b\[\d\d?m/g; // eslint-disable-line no-control-regex

function removeColors(str) {
  return str.replace(colorRegExp, '');
}

function isError(e) {
  return objectToString(e) === '[object Error]' || e instanceof Error;
}

function objectToString(o) {
  return Object.prototype.toString.call(o);
}

// Keep a list of deprecation codes that have been warned on so we only warn on
// each one once.
const codesWarned = {};

// Mark that a method should not be used.
// Returns a modified function which warns once by default.
// If --no-deprecation is set, then it is a no-op.
function deprecate(fn, msg, code) {
  if (process.noDeprecation === true) {
    return fn;
  }

  if (code !== undefined && typeof code !== 'string')
    throw new ERR_INVALID_ARG_TYPE('code', 'string', code);

  let warned = false;
  function deprecated(...args) {
    if (!warned) {
      warned = true;
      if (code !== undefined) {
        if (!codesWarned[code]) {
          process.emitWarning(msg, 'DeprecationWarning', code, deprecated);
          codesWarned[code] = true;
        }
      } else {
        process.emitWarning(msg, 'DeprecationWarning', deprecated);
      }
    }
    if (new.target) {
      return Reflect.construct(fn, args, new.target);
    }
    return fn.apply(this, args);
  }

  // The wrapper will keep the same prototype as fn to maintain prototype chain
  Object.setPrototypeOf(deprecated, fn);
  if (fn.prototype) {
    // Setting this (rather than using Object.setPrototype, as above) ensures
    // that calling the unwrapped constructor gives an instanceof the wrapped
    // constructor.
    deprecated.prototype = fn.prototype;
  }

  return deprecated;
}

function decorateErrorStack(err) {
  if (!(isError(err) && err.stack) ||
      getHiddenValue(err, kDecoratedPrivateSymbolIndex) === true)
    return;

  const arrow = getHiddenValue(err, kArrowMessagePrivateSymbolIndex);

  if (arrow) {
    err.stack = arrow + err.stack;
    setHiddenValue(err, kDecoratedPrivateSymbolIndex, true);
  }
}

function assertCrypto() {
  if (noCrypto)
    throw new ERR_NO_CRYPTO();
}

// Return undefined if there is no match.
// Move the "slow cases" to a separate function to make sure this function gets
// inlined properly. That prioritizes the common case.
function normalizeEncoding(enc) {
  if (enc == null || enc === 'utf8' || enc === 'utf-8') return 'utf8';
  return slowCases(enc);
}

function slowCases(enc) {
  switch (enc.length) {
    case 4:
      if (enc === 'UTF8') return 'utf8';
      if (enc === 'ucs2' || enc === 'UCS2') return 'utf16le';
      enc = `${enc}`.toLowerCase();
      if (enc === 'utf8') return 'utf8';
      if (enc === 'ucs2' || enc === 'UCS2') return 'utf16le';
      break;
    case 3:
      if (enc === 'hex' || enc === 'HEX' || `${enc}`.toLowerCase() === 'hex')
        return 'hex';
      break;
    case 5:
      if (enc === 'ascii') return 'ascii';
      if (enc === 'ucs-2') return 'utf16le';
      if (enc === 'UTF-8') return 'utf8';
      if (enc === 'ASCII') return 'ascii';
      if (enc === 'UCS-2') return 'utf16le';
      enc = `${enc}`.toLowerCase();
      if (enc === 'utf-8') return 'utf8';
      if (enc === 'ascii') return 'ascii';
      if (enc === 'usc-2') return 'utf16le';
      break;
    case 6:
      if (enc === 'base64') return 'base64';
      if (enc === 'latin1' || enc === 'binary') return 'latin1';
      if (enc === 'BASE64') return 'base64';
      if (enc === 'LATIN1' || enc === 'BINARY') return 'latin1';
      enc = `${enc}`.toLowerCase();
      if (enc === 'base64') return 'base64';
      if (enc === 'latin1' || enc === 'binary') return 'latin1';
      break;
    case 7:
      if (enc === 'utf16le' || enc === 'UTF16LE' ||
        `${enc}`.toLowerCase() === 'utf16le')
        return 'utf16le';
      break;
    case 8:
      if (enc === 'utf-16le' || enc === 'UTF-16LE' ||
        `${enc}`.toLowerCase() === 'utf-16le')
        return 'utf16le';
      break;
    default:
      if (enc === '') return 'utf8';
  }
}

function emitExperimentalWarning(feature) {
  if (experimentalWarnings.has(feature)) return;
  const msg = `${feature} is an experimental feature. This feature could ` +
       'change at any time';
  experimentalWarnings.add(feature);
  process.emitWarning(msg, 'ExperimentalWarning');
}

function filterDuplicateStrings(items, low) {
  const map = new Map();
  for (var i = 0; i < items.length; i++) {
    const item = items[i];
    const key = item.toLowerCase();
    if (low) {
      map.set(key, key);
    } else {
      map.set(key, item);
    }
  }
  return Array.from(map.values()).sort();
}

function cachedResult(fn) {
  let result;
  return () => {
    if (result === undefined)
      result = fn();
    return result.slice();
  };
}

// Useful for Wrapping an ES6 Class with a constructor Function that
// does not require the new keyword. For instance:
//   class A { constructor(x) {this.x = x;}}
//   const B = createClassWrapper(A);
//   B() instanceof A // true
//   B() instanceof B // true
function createClassWrapper(type) {
  function fn(...args) {
    return Reflect.construct(type, args, new.target || type);
  }
  // Mask the wrapper function name and length values
  Object.defineProperties(fn, {
    name: { value: type.name },
    length: { value: type.length }
  });
  Object.setPrototypeOf(fn, type);
  fn.prototype = type.prototype;
  return fn;
}

let signalsToNamesMapping;
function getSignalsToNamesMapping() {
  if (signalsToNamesMapping !== undefined)
    return signalsToNamesMapping;

  signalsToNamesMapping = Object.create(null);
  for (var key in signals) {
    signalsToNamesMapping[signals[key]] = key;
  }

  return signalsToNamesMapping;
}

function convertToValidSignal(signal) {
  if (typeof signal === 'number' && getSignalsToNamesMapping()[signal])
    return signal;

  if (typeof signal === 'string') {
    const signalName = signals[signal.toUpperCase()];
    if (signalName) return signalName;
  }

  throw new ERR_UNKNOWN_SIGNAL(signal);
}

function getConstructorOf(obj) {
  while (obj) {
    const descriptor = Object.getOwnPropertyDescriptor(obj, 'constructor');
    if (descriptor !== undefined &&
        typeof descriptor.value === 'function' &&
        descriptor.value.name !== '') {
      return descriptor.value;
    }

    obj = Object.getPrototypeOf(obj);
  }

  return null;
}

function getSystemErrorName(err) {
  const entry = errmap.get(err);
  return entry ? entry[0] : `Unknown system error ${err}`;
}

const kCustomPromisifiedSymbol = Symbol('util.promisify.custom');
const kCustomPromisifyArgsSymbol = Symbol('customPromisifyArgs');

function promisify(original) {
  if (typeof original !== 'function')
    throw new ERR_INVALID_ARG_TYPE('original', 'Function', original);

  if (original[kCustomPromisifiedSymbol]) {
    const fn = original[kCustomPromisifiedSymbol];
    if (typeof fn !== 'function') {
      throw new ERR_INVALID_ARG_TYPE('util.promisify.custom', 'Function', fn);
    }
    Object.defineProperty(fn, kCustomPromisifiedSymbol, {
      value: fn, enumerable: false, writable: false, configurable: true
    });
    return fn;
  }

  // Names to create an object from in case the callback receives multiple
  // arguments, e.g. ['stdout', 'stderr'] for child_process.exec.
  const argumentNames = original[kCustomPromisifyArgsSymbol];

  function fn(...args) {
    const promise = createPromise();
    try {
      original.call(this, ...args, (err, ...values) => {
        if (err) {
          promiseReject(promise, err);
        } else if (argumentNames !== undefined && values.length > 1) {
          const obj = {};
          for (var i = 0; i < argumentNames.length; i++)
            obj[argumentNames[i]] = values[i];
          promiseResolve(promise, obj);
        } else {
          promiseResolve(promise, values[0]);
        }
      });
    } catch (err) {
      promiseReject(promise, err);
    }
    return promise;
  }

  Object.setPrototypeOf(fn, Object.getPrototypeOf(original));

  Object.defineProperty(fn, kCustomPromisifiedSymbol, {
    value: fn, enumerable: false, writable: false, configurable: true
  });
  return Object.defineProperties(
    fn,
    Object.getOwnPropertyDescriptors(original)
  );
}

promisify.custom = kCustomPromisifiedSymbol;

// The build-in Array#join is slower in v8 6.0
function join(output, separator) {
  let str = '';
  if (output.length !== 0) {
    for (var i = 0; i < output.length - 1; i++) {
      // It is faster not to use a template string here
      str += output[i];
      str += separator;
    }
    str += output[i];
  }
  return str;
}

// As of V8 6.6, depending on the size of the array, this is anywhere
// between 1.5-10x faster than the two-arg version of Array#splice()
function spliceOne(list, index) {
  for (; index + 1 < list.length; index++)
    list[index] = list[index + 1];
  list.pop();
}

const kNodeModulesRE = /^(.*)[\\/]node_modules[\\/]/;

let getStructuredStack;

function isInsideNodeModules() {
  if (getStructuredStack === undefined) {
    // Lazy-load to avoid a circular dependency.
    const { runInNewContext } = require('vm');
    // Use `runInNewContext()` to get something tamper-proof and
    // side-effect-free. Since this is currently only used for a deprecated API,
    // the perf implications should be okay.
    getStructuredStack = runInNewContext(`(function() {
      Error.prepareStackTrace = function(err, trace) {
        err.stack = trace;
      };
      Error.stackTraceLimit = Infinity;

      return function structuredStack() {
        return new Error().stack;
      };
    })()`, {}, { filename: 'structured-stack' });
  }

  const stack = getStructuredStack();

  // Iterate over all stack frames and look for the first one not coming
  // from inside Node.js itself:
  if (Array.isArray(stack)) {
    for (const frame of stack) {
      const filename = frame.getFileName();
      // If a filename does not start with / or contain \,
      // it's likely from Node.js core.
      if (!/^\/|\\/.test(filename))
        continue;
      return kNodeModulesRE.test(filename);
    }
  }
  return false;
}


module.exports = {
  assertCrypto,
  cachedResult,
  convertToValidSignal,
  createClassWrapper,
  decorateErrorStack,
  deprecate,
  emitExperimentalWarning,
  filterDuplicateStrings,
  getConstructorOf,
  getSystemErrorName,
  isError,
  isInsideNodeModules,
  join,
  normalizeEncoding,
  objectToString,
  promisify,
  spliceOne,
  removeColors,

  // Symbol used to customize promisify conversion
  customPromisifyArgs: kCustomPromisifyArgsSymbol,

  // Symbol used to provide a custom inspect function for an object as an
  // alternative to using 'inspect'
  customInspectSymbol: Symbol('util.inspect.custom'),

  // Used by the buffer module to capture an internal reference to the
  // default isEncoding implementation, just in case userland overrides it.
  kIsEncodingSymbol: Symbol('kIsEncodingSymbol'),
  kExpandStackSymbol: Symbol('kExpandStackSymbol')
};
