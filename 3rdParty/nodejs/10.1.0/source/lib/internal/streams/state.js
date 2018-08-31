'use strict';

const { ERR_INVALID_OPT_VALUE } = require('internal/errors').codes;

function getHighWaterMark(state, options, duplexKey, isDuplex) {
  let hwm = options.highWaterMark;
  if (hwm != null) {
    if (typeof hwm !== 'number' || !(hwm >= 0))
      throw new ERR_INVALID_OPT_VALUE('highWaterMark', hwm);
    return Math.floor(hwm);
  } else if (isDuplex) {
    hwm = options[duplexKey];
    if (hwm != null) {
      if (typeof hwm !== 'number' || !(hwm >= 0))
        throw new ERR_INVALID_OPT_VALUE(duplexKey, hwm);
      return Math.floor(hwm);
    }
  }

  // Default value
  return state.objectMode ? 16 : 16 * 1024;
}

module.exports = {
  getHighWaterMark
};
