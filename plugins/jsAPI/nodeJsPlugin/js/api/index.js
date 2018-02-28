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

app.get('/', function(req, res, next) {
	var respObj = new resp(req);
	respObj.setDescription('ApertusVR API Home');
	res.send(respObj.toJSonString());
});

app.post('/setproperties', function(req, res, next) {
	var respObj = new resp(req);
	respObj.setDescription('Sets properties on several objects at the same time.');

	if (!respObj.validateHttpParams(req, res)) {
		res.status(400).send(respObj.toJSonString());
		return;
	}

	if (!req.body.data) {
		logger.error('no data in http req body');
		var errObj = errorMap.items.dataNotPresented;
		errObj.message = 'Data object is not presented in Http Request body.';
		respObj.addErrorItem(errObj);
		res.send(respObj.toJSonString());
		return;
	}

	if (!req.body.data.items) {
		logger.error('no items array in http req body.data');
		var errObj = errorMap.items.dataNotPresented;
		errObj.message = 'Items array is not presented in Http Request body.data.';
		respObj.addErrorItem(errObj);
		res.send(respObj.toJSonString());
		return;
	}

	for (var i = 0; i < req.body.data.items.length; i++) {
		var item = req.body.data.items[i];

		if (!item.type || !item.name) {
			logger.error('item has no type || name property');
			continue;
		}

		if (item.type == "node") {
			ape.nbind.JsBindManager().getNode(item.name, function(error, obj) {
				if (error) {
					respObj.addErrorItem({
						name: 'invalidCast',
						msg: obj,
						code: 666
					});
					res.status(400).send(respObj.toJSonString());
					return;
				}

				var q = utils.quaternionFromAngleAxis(item.properties.orientation.angle, item.properties.orientation.axis);
				obj.setOrientation(q);
			});
		}
	}

	respObj.addEventItem({
		group: 'PROPERTIES',
		type: 'PROPERTIES_SET',
		subjectName: ''
	});
	res.send(respObj.toJSonString());
});

module.exports = app;
