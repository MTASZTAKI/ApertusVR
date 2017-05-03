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

var ape = require('../ape.js');
var moduleManager = require('../helpers/module_manager/module_manager.js');
var express = moduleManager.requireNodeModule('express');
var app = express();
var utils = require('../helpers/utils/utils.js');
var logger = require("../helpers/logger/logger.js");

app.use('*', function(req, res, next) {
	console.log('all other routes');
	next({
		name: 'BadRequest',
		message: 'Invalid Url!',
		code: 404
	});
});

app.use(function(err, req, res, next) {
	console.log('error handling in server.js');
	console.log(err);
	var resObj = new utils.responseObj();
	if (err) {
		if (err.hasOwnProperty('name')) {
			if (err.name === 'UnauthorizedError') {
				if (!err.hasOwnProperty('message')) {
					err.message = 'The API request has invalid token!';
					err.code = 401;
					res.status(401);
				}
			} else if (err.name == 'BadRequest') {
				res.status(404);
			}
		}

		resObj.addErrorItem(err);
		res.send(resObj.toJSonString());
	}
});

module.exports = app;
