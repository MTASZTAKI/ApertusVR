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

var ape = require('../ape.js');
var moduleManager = require('../modules/module_manager/module_manager.js');
var express = moduleManager.requireNodeModule('express');
var app = express();
var utils = require('../modules/utils/utils.js');
var logger = require("../modules/log_manager/log_manager.js");
var resp = require('../modules/response_manager/response_manager.js');
var errorMap = require('../modules/utils/errors.js');

app.use('*', function(req, res, next) {
	next(errorMap.items.resourceNotFound);
});

app.use(function(err, req, res, next) {
	logger.debug('fallback catcherror: ', err);

	if (err) {
		var resObj = new resp(req);
		resObj.setDescription('Something wrong happened. :( Please check errors object for more info...');

		if (err.hasOwnProperty('name')) {
			if (errorMap.items.hasOwnProperty(err.name)) {
				if (!err.hasOwnProperty('message')) {
					err.message = errorMap.items[err.name].message;
				}
			}
			if (err.name == 'Error' && err.code == 'ENOENT') {
				err = errorMap.items.resourceNotFound;
			}
		}
		resObj.addErrorItem(err);
		res.status(resObj.getStatusCode());
		res.send(resObj.toJSonString());
	}
});

module.exports = app;
