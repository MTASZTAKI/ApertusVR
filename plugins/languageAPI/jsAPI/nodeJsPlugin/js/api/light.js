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

var url = require("url");
var path = require("path");
var util = require('util');
var ape = require('../ape.js');
var moduleManager = require('../modules/module_manager/module_manager.js');
var express = moduleManager.requireNodeModule('express');
var app = express();
var utils = require('../modules/utils/utils.js');
var logger = require("../modules/log_manager/log_manager.js");
var resp = require('../modules/response_manager/response_manager.js');
var errorMap = require('../modules/utils/errors.js');

exports.moduleTag = 'ApeHTTPApiLight';

app.post('/lights', function(req, res) {
	var respObj = new resp(req);
	respObj.setDescription('Creates a new light object with the specified name.');

	// handle http param validation errors
	req.checkBody('name', 'BodyParam is not presented').notEmpty()
	req.checkBody('name', 'BodyParam must be a string').isAlpha();
	if (!respObj.validateHttpParams(req, res)) {
		res.status(400).send(respObj.toJSonString());
		return;
	}

	// get name from url
	var name = req.body.name;
	var lightObj = ape.nbind.JsBindManager().createLight(name);
	respObj.addEvent({
		group: 'LIGHT',
		type: 'LIGHT_CREATE',
		subjectName: lightObj.getName()
	});
	res.send(respObj.toJSonString());
});

app.get('/lights/:name/diffusecolor', function(req, res) {
	var respObj = new resp(req);
	respObj.setDescription('Gets the diffusecolor of the specified light.');

	// handle http param validation errors
	req.checkParams('name', 'UrlParam is not presented').notEmpty()
	req.checkParams('name', 'UrlParam must be a string').isAlpha();
	if (!respObj.validateHttpParams(req, res)) {
		res.status(400).send(respObj.toJSonString());
		return;
	}

	// get entity name from url
	var name = req.params.name;
	var propertyName = path.basename(req.url).toLowerCase();

	ape.nbind.JsBindManager().getLight(
		name,
		function(error, obj) {
			if (error) {
				respObj.addErrorItem({
					name: 'invalidCast',
					msg: obj,
					code: 666
				});
				res.status(400).send(respObj.toJSonString());
			} else {
				var lightColor = obj.getDiffuseColor();
				var propertyValue = {
					r: lightColor.r,
					g: lightColor.g,
					b: lightColor.b,
					a: lightColor.a
				};
				var propObj = {};
				propObj[propertyName] = propertyValue;
				respObj.setData(propObj);
				res.send(respObj.toJSonString());
			}
		}
	);
});

app.post('/lights/:name/diffusecolor', function(req, res) {
	var respObj = new resp(req);
	respObj.setDescription('Sets the diffusecolor of the specified light.');

	// handle errors
	req.checkParams('name', 'UrlParam is not presented').notEmpty()
	req.checkParams('name', 'UrlParam must be a string').isAlpha();
	req.checkBody('r', 'BodyParam is not presented').notEmpty();
	req.checkBody('r', 'BodyParam must be a number').isInt();
	req.checkBody('g', 'BodyParam is not presented').notEmpty();
	req.checkBody('g', 'BodyParam must be a number').isInt();
	req.checkBody('b', 'BodyParam is not presented').notEmpty();
	req.checkBody('b', 'BodyParam must be a number').isInt();
	req.checkBody('a', 'BodyParam is not presented').notEmpty();
	req.checkBody('a', 'BodyParam must be a number').isInt();
	if (!respObj.validateHttpParams(req, res)) {
		res.status(400).send(respObj.toJSonString());
		return;
	}

	// get entity name from urlParam
	var name = req.params.name;
	logger.debug('setDiffuseColor() light name: ' + name);

	// get position from bodyParam (json)
	var lightColor = {
		r: req.body.r,
		g: req.body.g,
		b: req.body.b,
		a: req.body.a
	};

	var light = ape.nbind.JsBindManager().getLight(name);
	light.setDiffuseColor(new ape.nbind.Color(Number(lightColor.r), Number(lightColor.g), Number(lightColor.b), Number(lightColor.a)));

	// send back updated position
	res.send(JSON.stringify(lightColor));
});

module.exports = app;
