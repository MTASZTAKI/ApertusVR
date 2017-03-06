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

var utils = require('apertusvr/js/utils.js');

exports.pathMap = {};
exports.moduleTag = 'ApeHTTPApi';
exports.apiVersion = 'v1';
exports.rootPath = '/api/' + this.apiVersion;

var apeHTTPApiNode = require('apertusvr/js/apeHttpApi/apeHTTPApiNode.js');
exports.nodes = apeHTTPApiNode;

var apeHTTPApiLight = require('apertusvr/js/apeHttpApi/apeHTTPApiLight.js');
exports.lights = apeHTTPApiLight;

var apeHTTPApiText = require('apertusvr/js/apeHttpApi/apeHTTPApiText.js');
exports.texts = apeHTTPApiText;

exports.handleUnkownRes = function(req, res) {
  console.log('ape.httpApi.handleUnkownRes()');
  var respObj = new utils.responseObj();
  respObj.addError({
    name: 'unknownResource',
    msg: 'Resource ' + req.url + ' not found.',
    code: 69
  });
  res.send(respObj.toJSonString());
};

exports.registerPath = function(path, func) {
  this.pathMap[path] = func;
};
