'use strict';

const {
  errnoException,
  codes: {
    ERR_ASSERTION,
    ERR_CPU_USAGE,
    ERR_INVALID_ARG_TYPE,
    ERR_INVALID_ARRAY_LENGTH,
    ERR_INVALID_OPT_VALUE,
    ERR_UNCAUGHT_EXCEPTION_CAPTURE_ALREADY_SET,
    ERR_UNKNOWN_SIGNAL
  }
} = require('internal/errors');
const util = require('util');
const constants = process.binding('constants').os.signals;
const assert = require('assert').strict;
const { deprecate } = require('internal/util');

process.assert = deprecate(
  function(x, msg) {
    if (!x) throw new ERR_ASSERTION(msg || 'assertion error');
  },
  'process.assert() is deprecated. Please use the `assert` module instead.',
  'DEP0100');

function setup_performance() {
  require('perf_hooks');
}

// Set up the process.cpuUsage() function.
function setup_cpuUsage() {
  // Get the native function, which will be replaced with a JS version.
  const _cpuUsage = process.cpuUsage;

  // Create the argument array that will be passed to the native function.
  const cpuValues = new Float64Array(2);

  // Replace the native function with the JS version that calls the native
  // function.
  process.cpuUsage = function cpuUsage(prevValue) {
    // If a previous value was passed in, ensure it has the correct shape.
    if (prevValue) {
      if (!previousValueIsValid(prevValue.user)) {
        if (typeof prevValue !== 'object')
          throw new ERR_INVALID_ARG_TYPE('prevValue', 'object', prevValue);

        if (typeof prevValue.user !== 'number') {
          throw new ERR_INVALID_ARG_TYPE('prevValue.user',
                                         'number', prevValue.user);
        }
        throw new ERR_INVALID_OPT_VALUE.RangeError('prevValue.user',
                                                   prevValue.user);
      }

      if (!previousValueIsValid(prevValue.system)) {
        if (typeof prevValue.system !== 'number') {
          throw new ERR_INVALID_ARG_TYPE('prevValue.system',
                                         'number', prevValue.system);
        }
        throw new ERR_INVALID_OPT_VALUE.RangeError('prevValue.system',
                                                   prevValue.system);
      }
    }

    // Call the native function to get the current values.
    const errmsg = _cpuUsage(cpuValues);
    if (errmsg) {
      throw new ERR_CPU_USAGE(errmsg);
    }

    // If a previous value was passed in, return diff of current from previous.
    if (prevValue) {
      return {
        user: cpuValues[0] - prevValue.user,
        system: cpuValues[1] - prevValue.system
      };
    }

    // If no previous value passed in, return current value.
    return {
      user: cpuValues[0],
      system: cpuValues[1]
    };
  };

  // Ensure that a previously passed in value is valid. Currently, the native
  // implementation always returns numbers <= Number.MAX_SAFE_INTEGER.
  function previousValueIsValid(num) {
    return Number.isFinite(num) &&
        num <= Number.MAX_SAFE_INTEGER &&
        num >= 0;
  }
}

// The 3 entries filled in by the original process.hrtime contains
// the upper/lower 32 bits of the second part of the value,
// and the remaining nanoseconds of the value.
function setup_hrtime() {
  const _hrtime = process.hrtime;
  const hrValues = new Uint32Array(3);

  process.hrtime = function hrtime(time) {
    _hrtime(hrValues);

    if (time !== undefined) {
      if (!Array.isArray(time)) {
        throw new ERR_INVALID_ARG_TYPE('time', 'Array', time);
      }
      if (time.length !== 2) {
        throw new ERR_INVALID_ARRAY_LENGTH('time', 2, time.length);
      }

      const sec = (hrValues[0] * 0x100000000 + hrValues[1]) - time[0];
      const nsec = hrValues[2] - time[1];
      const needsBorrow = nsec < 0;
      return [needsBorrow ? sec - 1 : sec, needsBorrow ? nsec + 1e9 : nsec];
    }

    return [
      hrValues[0] * 0x100000000 + hrValues[1],
      hrValues[2]
    ];
  };
}

function setupMemoryUsage() {
  const memoryUsage_ = process.memoryUsage;
  const memValues = new Float64Array(4);

  process.memoryUsage = function memoryUsage() {
    memoryUsage_(memValues);
    return {
      rss: memValues[0],
      heapTotal: memValues[1],
      heapUsed: memValues[2],
      external: memValues[3]
    };
  };
}

function setupConfig(_source) {
  // NativeModule._source
  // used for `process.config`, but not a real module
  const config = _source.config;
  delete _source.config;

  process.config = JSON.parse(config, function(key, value) {
    if (value === 'true') return true;
    if (value === 'false') return false;
    return value;
  });
}


function setupKillAndExit() {

  process.exit = function(code) {
    if (code || code === 0)
      process.exitCode = code;

    if (!process._exiting) {
      process._exiting = true;
      process.emit('exit', process.exitCode || 0);
    }
    process.reallyExit(process.exitCode || 0);
  };

  process.kill = function(pid, sig) {
    var err;

    // eslint-disable-next-line eqeqeq
    if (pid != (pid | 0)) {
      throw new ERR_INVALID_ARG_TYPE('pid', 'number', pid);
    }

    // preserve null signal
    if (sig === (sig | 0)) {
      err = process._kill(pid, sig);
    } else {
      sig = sig || 'SIGTERM';
      if (constants[sig]) {
        err = process._kill(pid, constants[sig]);
      } else {
        throw new ERR_UNKNOWN_SIGNAL(sig);
      }
    }

    if (err)
      throw errnoException(err, 'kill');

    return true;
  };
}


function setupSignalHandlers() {
  const signalWraps = Object.create(null);
  let Signal;

  function isSignal(event) {
    return typeof event === 'string' && constants[event] !== undefined;
  }

  // Detect presence of a listener for the special signal types
  process.on('newListener', function(type) {
    if (isSignal(type) && signalWraps[type] === undefined) {
      if (Signal === undefined)
        Signal = process.binding('signal_wrap').Signal;
      const wrap = new Signal();

      wrap.unref();

      wrap.onsignal = process.emit.bind(process, type, type);

      const signum = constants[type];
      const err = wrap.start(signum);
      if (err) {
        wrap.close();
        throw errnoException(err, 'uv_signal_start');
      }

      signalWraps[type] = wrap;
    }
  });

  process.on('removeListener', function(type) {
    if (signalWraps[type] !== undefined && this.listenerCount(type) === 0) {
      signalWraps[type].close();
      delete signalWraps[type];
    }
  });
}


function setupChannel() {
  // If we were spawned with env NODE_CHANNEL_FD then load that up and
  // start parsing data from that stream.
  if (process.env.NODE_CHANNEL_FD) {
    const fd = parseInt(process.env.NODE_CHANNEL_FD, 10);
    assert(fd >= 0);

    // Make sure it's not accidentally inherited by child processes.
    delete process.env.NODE_CHANNEL_FD;

    require('child_process')._forkChild(fd);
    assert(process.send);
  }
}


function setupRawDebug() {
  const rawDebug = process._rawDebug;
  process._rawDebug = function() {
    rawDebug(util.format.apply(null, arguments));
  };
}


function setupUncaughtExceptionCapture(exceptionHandlerState) {
  // This is a typed array for faster communication with JS.
  const shouldAbortOnUncaughtToggle = process._shouldAbortOnUncaughtToggle;
  delete process._shouldAbortOnUncaughtToggle;

  process.setUncaughtExceptionCaptureCallback = function(fn) {
    if (fn === null) {
      exceptionHandlerState.captureFn = fn;
      shouldAbortOnUncaughtToggle[0] = 1;
      return;
    }
    if (typeof fn !== 'function') {
      throw new ERR_INVALID_ARG_TYPE('fn', ['Function', 'null'], fn);
    }
    if (exceptionHandlerState.captureFn !== null) {
      throw new ERR_UNCAUGHT_EXCEPTION_CAPTURE_ALREADY_SET();
    }
    exceptionHandlerState.captureFn = fn;
    shouldAbortOnUncaughtToggle[0] = 0;
  };

  process.hasUncaughtExceptionCaptureCallback = function() {
    return exceptionHandlerState.captureFn !== null;
  };
}

module.exports = {
  setup_performance,
  setup_cpuUsage,
  setup_hrtime,
  setupMemoryUsage,
  setupConfig,
  setupKillAndExit,
  setupSignalHandlers,
  setupChannel,
  setupRawDebug,
  setupUncaughtExceptionCapture
};
