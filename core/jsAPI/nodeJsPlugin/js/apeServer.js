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
var expressValidator = require('express-validator');
var apePluginLoader = require('apertusvr/js/apePluginLoader');
var apeHTTPApi = require('apertusvr/js/apeHttpApi');

const moduleTag = 'NodeJsExt';
const apiVersion = 'v1';
const rootPath = '/api/' + apiVersion;

// set server host and port
var host = "0.0.0.0" || process.env.VCAP_APP_HOST || process.env.HOST || 'localhost';
var port = process.env.VCAP_APP_PORT || process.env.PORT || 3000;

// create Express server
var app = express();

app['log'] = function(tag, str) {
  console.log(moduleTag + ': ' + tag + ': ' + str);
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
  log('Error', error.message);
  res.status(err.status || 500);
  res.send(JSON.stringify(err));
});


// set root uri
app.get('/', function(req, res, next) {
    res.end("ApertusVR Home");
});

// create new node
// example:
// curl --silent --header "Content-Type: application/json" -X POST --data '{"name": "testNode"}' http://localhost:3000/api/v1/nodes/
app.post(rootPath + '/nodes', function(req, res, next) {
  apeHTTPApi.create(req, res, next);
});

// get position of an existing node
// example:
// curl --silent --header "Content-Type: application/json" -X GET http://localhost:3000/api/v1/nodes/testNode/position
app.get(rootPath + '/nodes/:name/position', function(req, res, next) {
  apeHTTPApi.getPosition(req, res, next);
});

// set position of an existing node
// example:
// curl --silent --header "Content-Type: application/json" -X POST --data '{"x": 10, "y": 20, "z": 30}' http://localhost:3000/api/v1/nodes/testNode/position
app.post(rootPath + '/nodes/:name/position', function(req, res, next) {
  apeHTTPApi.setPosition(req, res, next);
});

app.log('app', 'ApertusVR NodeJS Server');
app.listen(port, host,  function() {
  app.log('app', 'Listening on ' + host + ':' + port);
});
