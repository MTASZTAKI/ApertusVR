/*MIT License

Copyright (c) 2017 MTA SZTAKI

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
var ape = require('../ape.js');
var moduleManager = require('../modules/module_manager/module_manager.js');
var express = moduleManager.requireNodeModule('express');
var app = express();
var async = moduleManager.requireNodeModule('async');
var utils = require('../modules/utils/utils.js');
var logger = require("../modules/log_manager/log_manager.js");
var resp = require('../modules/response_manager/response_manager.js');
var errorMap = require('../modules/utils/errors.js');
var path = moduleManager.requireNodeModule('path');
var formidable = moduleManager.requireNodeModule('formidable');
var fs = require('fs');

exports.moduleTag = 'ApeHTTPApiFileGeometry';

app.post('/filegeometries/', function(req, res){

    // create an incoming form object
    var form = new formidable.IncomingForm();
  
    // specify that we want to allow the user to upload multiple files in a single request
    form.multiples = false;
  
    // store all uploads in the /uploads directory
    form.uploadDir = path.join(moduleManager.configurationPath, 'uploads');
    var fullFilePath;

    // every time a file has been uploaded successfully,
    // rename it to it's orignal name
    form.on('file', function(field, file) {
      fullFilePath = path.join(form.uploadDir, file.name);
      fs.rename(file.path, fullFilePath, function(err){
        if (err)
          logger.error('An error has occured: \n' + err);
      });
    });
  
    // log any errors that occur
    form.on('error', function(err) {
      logger.error('An error has occured: \n' + err);
    });
  
    // once all the files have been uploaded, send a response to the client
    form.on('end', function() {
      res.end('success');
      logger.debug('file upload success: ', fullFilePath);
      var fileGeometry = ape.nbind.JsBindManager().createFileGeometry('robotFileGeometry');
      fileGeometry.setFileName(fullFilePath);
    });
  
    // parse the incoming request containing the form data
    form.parse(req);
});

module.exports = app;
