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
var ape = require('apertusvr/js/ape');
var utils = require('apertusvr/js/utils.js');

exports.moduleTag = 'ApeHTTPApiNode';

exports.create = function(req, res, next) {
  console.log('ape.httpApi.nodes.create()');
  var respObj = new utils.responseObj();

  // handle http param validation errors
  req.checkBody('name', 'BodyParam is not presented').notEmpty()
  req.checkBody('name', 'BodyParam must be a string').isAlpha();
  if (!respObj.validateHttpParams(req, res)) {
    res.status(400).send(respObj.toJSonString());
    return;
  }

  // get name from url
  var name = req.body.name;
  var nodeObj = ape.nbind.JsBindManager().createNode(name);
  respObj.addEvent({
    group: 'NODE',
    type: 'NODE_CREATE',
    subjectName: nodeObj.getName()
  });
  res.send(respObj.toJSonString());
};

exports.getPosition = function(req, res, next) {
  console.log('ape.httpApi.nodes.getPosition()');
  var respObj = new utils.responseObj();

  // handle http param validation errors
  req.checkParams('name', 'UrlParam is not presented').notEmpty()
  req.checkParams('name', 'UrlParam must be a string').isAlpha();
  if (!respObj.validateHttpParams(req, res)) {
    res.status(400).send(respObj.toJSonString());
    return;
  }

  // get name from url
  var name = req.params.name;

  ape.nbind.JsBindManager().getNode(name, function(error, obj) {
    if (error) {
      respObj.addError({
        name: 'invalidCast',
        msg: obj,
        code: 666
      });
      res.status(400).send(respObj.toJSonString());
      return;
    }

    respObj.setData({
      position: utils.convertToJsObj(obj.getPosition())
    });
    res.send(respObj.toJSonString());
  });
};

exports.setPosition = function(req, res, next) {
  console.log('ape.httpApi.nodes.setPosition()');
  var respObj = new utils.responseObj();

  // handle http param validation errors
  req.checkParams('name', 'UrlParam is not presented').notEmpty()
  req.checkParams('name', 'UrlParam must be a string').isAlpha();
  req.checkBody('x', 'BodyParam is not presented').notEmpty();
  req.checkBody('y', 'BodyParam is not presented').notEmpty();
  req.checkBody('z', 'BodyParam is not presented').notEmpty();
  if (!respObj.validateHttpParams(req, res)) {
    res.status(400).send(respObj.toJSonString());
    return;
  }

  // get node name from urlParam
  var name = req.params.name;

  ape.nbind.JsBindManager().getNode(name, function(error, obj) {
    if (error) {
      respObj.addError({
        name: 'invalidCast',
        msg: obj,
        code: 666
      });
      res.status(400).send(respObj.toJSonString());
      return;
    }

    var newPos = new ape.nbind.Vector3(Number(req.body.x), Number(req.body.y), Number(req.body.z));
    // if (newPos.notEqualTo(obj.getPosition())) {
    //   respObj.addEvent({
    //     group: 'NODE',
    //     type: 'NODE_POSITION',
    //     subjectName: obj.getName()
    //   });
    // }
    obj.setPosition(newPos);
    respObj.setData({
      position: utils.convertToJsObj(obj.getPosition())
    });
    res.send(respObj.toJSonString());
  });
};

exports.getOrientation = function(req, res, next) {
  console.log('ape.httpApi.nodes.getOrientation()');
  var respObj = new utils.responseObj();

  // handle http param validation errors
  req.checkParams('name', 'UrlParam is not presented').notEmpty()
  req.checkParams('name', 'UrlParam must be a string').isAlpha();
  if (!respObj.validateHttpParams(req, res)) {
    res.status(400).send(respObj.toJSonString());
    return;
  }

  // get name from url
  var name = req.params.name;

  ape.nbind.JsBindManager().getNode(name, function(error, obj) {
    if (error) {
      respObj.addError({
        name: 'invalidCast',
        msg: obj,
        code: 666
      });
      res.status(400).send(respObj.toJSonString());
      return;
    }

    respObj.setData({
      orientation: utils.convertToJsObj(obj.getOrientation())
    });
    res.send(respObj.toJSonString());
  });
};

exports.setOrientation = function(req, res, next) {
  console.log('ape.httpApi.nodes.setOrientation()');
  var respObj = new utils.responseObj();

  // handle http param validation errors
  req.checkParams('name', 'UrlParam is not presented').notEmpty()
  req.checkParams('name', 'UrlParam must be a string').isAlpha();
  req.checkBody('w', 'BodyParam is not presented').notEmpty();
  req.checkBody('x', 'BodyParam is not presented').notEmpty();
  req.checkBody('y', 'BodyParam is not presented').notEmpty();
  req.checkBody('z', 'BodyParam is not presented').notEmpty();
  if (!respObj.validateHttpParams(req, res)) {
    res.status(400).send(respObj.toJSonString());
    return;
  }

  // get node name from urlParam
  var name = req.params.name;

  ape.nbind.JsBindManager().getNode(name, function(error, obj) {
    if (error) {
      respObj.addError({
        name: 'invalidCast',
        msg: obj,
        code: 666
      });
      res.status(400).send(respObj.toJSonString());
      return;
    }

    var newOrt = new ape.nbind.Quaternion(Number(req.body.w), Number(req.body.x), Number(req.body.y), Number(req.body.z));
    // if (newOrt.notEqualTo(obj.getOrientation())) {
    //   respObj.addEvent({
    //     group: 'NODE',
    //     type: 'NODE_ORIENTATION',
    //     subjectName: obj.getName()
    //   });
    // }
    obj.setOrientation(newOrt);
    respObj.setData({
      orientation: utils.convertToJsObj(obj.getOrientation())
    });
    res.send(respObj.toJSonString());
  });
};
