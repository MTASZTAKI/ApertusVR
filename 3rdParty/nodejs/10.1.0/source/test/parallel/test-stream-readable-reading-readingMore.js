'use strict';
const common = require('../common');
const assert = require('assert');
const Readable = require('stream').Readable;

{
  const readable = new Readable({
    read(size) {}
  });

  const state = readable._readableState;

  // Starting off with false initially.
  assert.strictEqual(state.reading, false);
  assert.strictEqual(state.readingMore, false);

  readable.on('data', common.mustCall((data) => {
    // while in a flowing state with a 'readable' listener
    // we should not be reading more
    if (readable.readableFlowing)
      assert.strictEqual(state.readingMore, true);

    // reading as long as we've not ended
    assert.strictEqual(state.reading, !state.ended);
  }, 2));

  function onStreamEnd() {
    // End of stream; state.reading is false
    // And so should be readingMore.
    assert.strictEqual(state.readingMore, false);
    assert.strictEqual(state.reading, false);
  }

  readable.on('readable', common.mustCall(() => {
    // 'readable' always gets called before 'end'
    // since 'end' hasn't been emitted, more data could be incoming
    assert.strictEqual(state.readingMore, true);

    // if the stream has ended, we shouldn't be reading
    assert.strictEqual(state.ended, !state.reading);

    const data = readable.read();
    if (data === null) // reached end of stream
      process.nextTick(common.mustCall(onStreamEnd, 1));
  }, 2));

  readable.on('end', common.mustCall(onStreamEnd));
  readable.push('pushed');

  readable.read(6);

  // reading
  assert.strictEqual(state.reading, true);
  assert.strictEqual(state.readingMore, true);

  // add chunk to front
  readable.unshift('unshifted');

  // end
  readable.push(null);
}

{
  const readable = new Readable({
    read(size) {}
  });

  const state = readable._readableState;

  // Starting off with false initially.
  assert.strictEqual(state.reading, false);
  assert.strictEqual(state.readingMore, false);

  readable.on('data', common.mustCall((data) => {
    // while in a flowing state without a 'readable' listener
    // we should be reading more
    if (readable.readableFlowing)
      assert.strictEqual(state.readingMore, true);

    // reading as long as we've not ended
    assert.strictEqual(state.reading, !state.ended);
  }, 2));

  function onStreamEnd() {
    // End of stream; state.reading is false
    // And so should be readingMore.
    assert.strictEqual(state.readingMore, false);
    assert.strictEqual(state.reading, false);
  }

  readable.on('end', common.mustCall(onStreamEnd));
  readable.push('pushed');

  // stop emitting 'data' events
  assert.strictEqual(state.flowing, true);
  readable.pause();

  // paused
  assert.strictEqual(state.reading, false);
  assert.strictEqual(state.flowing, false);

  readable.resume();
  assert.strictEqual(state.reading, false);
  assert.strictEqual(state.flowing, true);

  // add chunk to front
  readable.unshift('unshifted');

  // end
  readable.push(null);
}

{
  const readable = new Readable({
    read(size) {}
  });

  const state = readable._readableState;

  // Starting off with false initially.
  assert.strictEqual(state.reading, false);
  assert.strictEqual(state.readingMore, false);

  const onReadable = common.mustNotCall;

  readable.on('readable', onReadable);

  readable.on('data', common.mustCall((data) => {
    // reading as long as we've not ended
    assert.strictEqual(state.reading, !state.ended);
  }, 2));

  readable.removeListener('readable', onReadable);

  function onStreamEnd() {
    // End of stream; state.reading is false
    // And so should be readingMore.
    assert.strictEqual(state.readingMore, false);
    assert.strictEqual(state.reading, false);
  }

  readable.on('end', common.mustCall(onStreamEnd));
  readable.push('pushed');

  // we are still not flowing, we will be resuming in the next tick
  assert.strictEqual(state.flowing, false);

  // wait for nextTick, so the readableListener flag resets
  process.nextTick(function() {
    readable.resume();

    // stop emitting 'data' events
    assert.strictEqual(state.flowing, true);
    readable.pause();

    // paused
    assert.strictEqual(state.flowing, false);

    readable.resume();
    assert.strictEqual(state.flowing, true);

    // add chunk to front
    readable.unshift('unshifted');

    // end
    readable.push(null);
  });
}
