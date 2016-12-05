/*MIT License

Copyright (c) 2016 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

var util = require('util');
var expressValidator = require('express-validator');
var ape = require('apertusvr/apertusvr.node');

// create ape singleton variables
var sceneMgr = ape.IScene.getSingleton(); // IScene*
var eventMgr = ape.IEventManager.getSingleton(); // IEventManager*

// TODO: fix event enums in apertusvr_wrap.cxx
// eventMgr.connectEvent(ape.EVENT_NODEGROUP, onEvent);

const moduleTag = 'NodeJsExt';
const apiVersion = 'v1';
const rootPath = '/api/' + apiVersion;

// create ape eventGroup string mappings
var eventGroup = {};
// TODO: fix event enums in apertusvr_wrap.cxx
// eventGroup[ape.EVENT_SCENEGROUP] = nameOf(function() { return ape.EVENT_SCENEGROUP; });
// eventGroup[ape.EVENT_NODEGROUP] = nameOf(function() { return ape.EVENT_NODEGROUP; });
// eventGroup[ape.EVENT_LIGHTGROUP] = nameOf(function() { return ape.EVENT_LIGHTGROUP; });
// eventGroup[ape.EVENT_VISUALGROUP] = nameOf(function() { return ape.EVENT_VISUALGROUP; });
// eventGroup[ape.EVENT_MANUALVISUALGROUP] = nameOf(function() { return ape.EVENT_MANUALVISUALGROUP; });
// eventGroup[ape.EVENT_LABELGROUP] = nameOf(function() { return ape.EVENT_LABELGROUP; });
// eventGroup[ape.EVENT_INVALIDGROUP] = nameOf(function() { return ape.EVENT_INVALIDGROUP; });

// encapsulate console.log()
function log(tag, str) {
  console.log(moduleTag + ': ' + tag + ': ' + str);
}

// return the json string representation of an object
function inspect(obj) {
  return util.inspect(obj, {depth: null}); // null = no depth limit
}

// return the name of a variable / expression
function nameOf (exp) {
  return exp.toString().match(/[.](\w+)/)[1];
};

// handle ape events
function onEvent(event) {
  log('onEvent()', 'event: ' + JSON.stringify(event));
  log('onEvent()', 'group: ' + event.group + ' (' + eventGroup[event.group] + ')');
  log('onEvent()', 'type: ' + event.type);

  // example handling of an event
  if (event.type == ape.EVENT_NODE_CREATE) {
    // ...
  }
}

exports.create = function(req, res, next) {
  // handle errors
  req.checkBody('name', 'BodyParam is not presented').notEmpty()
  req.checkBody('name', 'BodyParam must be a string').isAlpha();

  var errors = req.validationErrors();
  if (errors) {
    res.status(400).send('There have been validation errors: ' + util.inspect(errors));
    return;
  }

  // get name from url
  var name = req.body.name;
  var nodeWeakPtr = sceneMgr.createNode(name);

  //res.write('Node "' + name + '" created.');
  res.write('Node "' + nodeWeakPtr.getName() + '" created.');
  res.end();
};

exports.getPosition = function(req, res, next) {
  // handle errors
  req.checkParams('name', 'UrlParam is not presented').notEmpty()
  req.checkParams('name', 'UrlParam must be a string').isAlpha();

  var errors = req.validationErrors();
  if (errors) {
    res.status(400).send('There have been validation errors: ' + inspect(errors));
    return;
  }

  // get name from url
  var name = req.params.name;

  var nodeWeakPtr = sceneMgr.getNode(name);
  res.send(inspect(nodeWeakPtr.getPosition()));
};

exports.setPosition = function(req, res, next) {
  log('req.body: ', inspect(req.body));

  // handle errors
  req.checkParams('name', 'UrlParam is not presented').notEmpty()
  req.checkParams('name', 'UrlParam must be a string').isAlpha();

  req.checkBody('x', 'BodyParam is not presented').notEmpty();
  req.checkBody('x', 'BodyParam must be a number').isInt();
  req.checkBody('y', 'BodyParam is not presented').notEmpty();
  req.checkBody('y', 'BodyParam must be a number').isInt();
  req.checkBody('z', 'BodyParam is not presented').notEmpty();
  req.checkBody('z', 'BodyParam must be a number').isInt();

  var errors = req.validationErrors();
  if (errors) {
    res.status(400).send('There have been validation errors: ' + inspect(errors));
    return;
  }

  // get node name from urlParam
  var name = req.params.name;

  // get position from bodyParam (json)
  var pos = {
    x: req.body.x,
    y: req.body.y,
    z: req.body.z
  };

  var nodeWeakPtr = sceneMgr.getNode(name);
  nodeWeakPtr.setPosition(new ape.Vector3(Number(pos.x), Number(pos.y), Number(pos.z)));

  // send back updated position
  res.send(inspect(nodeWeakPtr.getPosition()));
};
