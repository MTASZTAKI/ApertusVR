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

var express = require('express');
var bodyParser = require('body-parser');
var expressValidator = require('express-validator');
var fs = require('fs');

// load ApertusVR
var config = require('./config.json');
if (!fs.existsSync(config.sourcePathJs)) {
	console.log('error: source path does not exists: ', config.sourcePathJs);
	process.exit(1);
}

var moduleManager = require(config.sourcePathJs + '/modules/module_manager/module_manager.js');
moduleManager.setConfigType(config.configuration);
var ape = require(config.sourcePathJs + 'ape.js');
var logger = require(config.sourcePathJs + "/modules/log_manager/log_manager.js");
logger.debug('config:', config);

// set host and port
moduleManager.httpServer.host = "0.0.0.0" || process.env.VCAP_APP_HOST || process.env.HOST || 'localhost';
moduleManager.httpServer.port = process.argv[2] || process.env.VCAP_APP_PORT || process.env.PORT || 3000;
moduleManager.httpServer.address = moduleManager.httpServer.host + ":" + moduleManager.httpServer.port;

var app = express();
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({extended: true}));
app.use(expressValidator());

app.listen(moduleManager.httpServer.port, moduleManager.httpServer.host, function() {
	logger.debug('Listening on ' + moduleManager.httpServer.host + ':' + moduleManager.httpServer.port);
	ape.start(app);
});
