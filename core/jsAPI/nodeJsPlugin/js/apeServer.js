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

var http = require("http");
var express = require('express');
var bodyParser = require('body-parser');
var logger = require('morgan');
var assert = require('assert');
var util = require('util');
var utils = require('apertusvr/js/utils.js');
var expressValidator = require('express-validator');
var ape = require('apertusvr/js/ape.js');
var x3dLoader = require('apertusvr/js/plugins/x3dLoader/x3dLoader.js');

const moduleTag = 'NodeJsExt';
const apiVersion = 'v1';
const rootPath = '/api/' + apiVersion;
const nodesPath = rootPath + '/nodes';
const lightsPath = rootPath + '/lights';
const textsPath = rootPath + '/texts';

// set server host and port
var host = "0.0.0.0" || process.env.VCAP_APP_HOST || process.env.HOST || 'localhost';
var port = process.env.VCAP_APP_PORT || process.env.PORT || 3000;

// create Express server
var app = express();

app['log'] = function(tag, str) {
  console.log(moduleTag + ': ' + tag + ': ' + str);
}

ape.httpMethodEnum = {
  GET: 'get',
  POST: 'post',
  PUT: 'put'
}

function registerHttpPath(method, path, func) {
  ape.httpApi.registerPath(path, func);
  app[method](path, func);
}

// use logger
app.use(logger('dev'));
// parse application/json
app.use(bodyParser.json());
// parse application/x-www-form-urlencoded
app.use(bodyParser.urlencoded({ extended: true }));
// this line must be immediately after express.bodyParser()!
app.use(expressValidator());


// APP-ROUTER
// ==========

// production error handler
// no stacktraces leaked to user
app.use(function(err, req, res, next) {
  app.log('Error', error.message);
  res.status(err.status || 500);
  res.send(JSON.stringify(err));
});

// app.use(function(req, res) {
//   ape.httpApi.handleUnkownRes(req, res);
// });

// set root uri
app.get('/', function(req, res, next) {
  res.end("ApertusVR Home");
});


// Nodes
// curl --silent --header "Content-Type: application/json" -X POST http://localhost:3000/api/v1/nodes/ --data '{"name": "testNode"}' | python -m json.tool
registerHttpPath(ape.httpMethodEnum.POST, nodesPath, ape.httpApi.nodes.create);

// curl --silent --header "Content-Type: application/json" -X GET http://localhost:3000/api/v1/nodes/testNode/position | python -m json.tool
registerHttpPath(ape.httpMethodEnum.GET, nodesPath + '/:name/position', ape.httpApi.nodes.getPosition);

// curl --silent --header "Content-Type: application/json" -X POST http://localhost:3000/api/v1/nodes/testNode/position --data '{"x": 10, "y": 20, "z": 30}' | python -m json.tool
registerHttpPath(ape.httpMethodEnum.POST, nodesPath + '/:name/position', ape.httpApi.nodes.setPosition);


// Lights
// curl --silent --header "Content-Type: application/json" -X POST http://localhost:3000/api/v1/lights/ --data '{"name": "testLight"}' | python -m json.tool
registerHttpPath(ape.httpMethodEnum.POST, lightsPath, ape.httpApi.lights.create);

// curl --silent --header "Content-Type: application/json" -X GET http://localhost:3000/api/v1/lights/testLight/diffusecolor | python -m json.tool
registerHttpPath(ape.httpMethodEnum.GET, lightsPath + '/:name/diffusecolor', ape.httpApi.lights.getDiffuseColor);

// curl --silent --header "Content-Type: application/json" -X POST http://localhost:3000/api/v1/nodes/testNode/position --data '{"r": 1, "g": 2, "b": 3, "a": 1}' | python -m json.tool
registerHttpPath(ape.httpMethodEnum.POST, nodesPath + '/:name/diffusecolor', ape.httpApi.lights.setDiffuseColor);

// Texts
// curl --silent --header "Content-Type: application/json" -X POST http://localhost:3000/api/v1/texts/ --data '{"name": "testText"}' | python -m json.tool
registerHttpPath(ape.httpMethodEnum.POST, textsPath, ape.httpApi.texts.create);

// curl --silent --header "Content-Type: application/json" -X GET http://localhost:3000/api/v1/texts/testText/caption | python -m json.tool
registerHttpPath(ape.httpMethodEnum.GET, textsPath + '/:name/caption', ape.httpApi.texts.getCaption);

// curl --silent --header "Content-Type: application/json" -X POST http://localhost:3000/api/v1/nodes/testText/caption --data '{"caption": "testCaption"}' | python -m json.tool
registerHttpPath(ape.httpMethodEnum.POST, textsPath + '/:name/caption', ape.httpApi.texts.setCaption);


app.log('app', 'ApertusVR NodeJS Server');
app.listen(port, host,  function() {
  app.log('app', 'Listening on ' + host + ':' + port);
  ape.jsPluginLoader.loadPlugins();
  app.log('app', '\n' + utils.inspect(ape));

  // ApeColor tests
  // var apeColor1 = ape.nbind.Color();
  // utils.iterate(apeColor1, 'apeColor1', '');
  // console.log('apeColor1 toString(): ' + apeColor1.toString());
  // console.log('apeColor1 r: ' + apeColor1.r + ' g: ' + apeColor1.g + ' b: ' + apeColor1.b + ' a: ' + apeColor1.a);

  // var apeColor2 = ape.nbind.Color(1, 2, 3, 4);
  // utils.iterate(apeColor2, 'apeColor2', '');
  // console.log('apeColor2 toString(): ' + apeColor2.toString());
  // console.log('apeColor2 r: ' + apeColor2.r + ' g: ' + apeColor2.g + ' b: ' + apeColor2.b + ' a: ' + apeColor2.a);

  var jsBindManager = ape.nbind.JsBindManager();
  utils.iterate(jsBindManager, 'jsBindManager', '');

  // if demoObjectNode found, attach a textGeometry and set caption
  ape.nbind.JsBindManager().getNode('demoObjectNode', function(error, obj) {
    if (error) {
      console.log('error: ' + error);
      return;
    }
    var textObj = jsBindManager.createText('textGeometry');
    textObj.setParentNodeJsPtr(obj);

    setInterval(function() {
      var pos = obj.getPosition();
      var ort = obj.getOrientation();
      textObj.setCaption('x: ' + roundDecimal(pos.x) + ', y: ' + roundDecimal(pos.y) + ', z: ' + roundDecimal(pos.z) + '\n' +
                         'w: ' + roundDecimal(ort.w) + ', x: ' + roundDecimal(ort.x) + ', y: ' + roundDecimal(ort.y) + ', z: ' + roundDecimal(ort.z)
      );
    }, 20);

  });

  x3dLoader.parseX3D('node_modules/apertusvr/js/plugins/x3dLoader/samples/Manipulator.x3d');
  console.log('X3D-parse done.');
});

function roundDecimal(num) {
  return (Math.round(num * 100) / 100).toFixed(2);
};
