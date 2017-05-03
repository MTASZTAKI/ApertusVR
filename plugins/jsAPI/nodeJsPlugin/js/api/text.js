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
var ape = require('../ape.js');
var utils = require('../utils.js');

var express = ape.requireNodeModule('express');
var app = express();

exports.moduleTag = 'ApeHTTPApiText';

app.post('/texts', function(req, res) {
	console.log('ape.httpApi.texts.create()');
	var respObj = new utils.responseObj();

	// handle http param validation errors
	req.checkBody('name', 'BodyParam is not presented').notEmpty();
	req.checkBody('name', 'BodyParam must be a string').isAlpha();
	if (!respObj.validateHttpParams(req, res)) {
		res.status(400).send(respObj.toJSonString());
		return;
	}

	// get name from url
	var name = req.body.name;
	var textObj = ape.nbind.JsBindManager().createText(name);
	respObj.addEvent({
		group: 'TEXT',
		type: 'TEXT_CREATE',
		subjectName: textObj.getName()
	});
	res.send(respObj.toJSonString());
});

app.get('/texts/:name/caption', function(req, res) {
	console.log('ape.httpApi.texts.getCaption()');
	var respObj = new utils.responseObj();

	// handle http param validation errors
	req.checkParams('name', 'UrlParam is not presented').notEmpty()
	req.checkParams('name', 'UrlParam must be a string').isAlpha();
	if (!respObj.validateHttpParams(req, res)) {
		res.status(400).send(respObj.toJSonString());
		return;
	}

	// get entity name from url
	var name = req.params.name;

	ape.nbind.JsBindManager().getText(name, function(error, obj) {
		if (error) {
			respObj.addErrorItem({
				name: 'invalidCast',
				msg: obj,
				code: 666
			});
			res.status(400).send(respObj.toJSonString());
			return;
		}

		respObj.setData({
			caption: obj.getCaption()
		});
		res.send(respObj.toJSonString());
	});
});

app.post('/texts/:name/caption', function(req, res) {
	console.log('ape.httpApi.texts.setCaption()');
	var respObj = new utils.responseObj();

	// handle http param validation errors
	req.checkParams('name', 'UrlParam is not presented').notEmpty();
	req.checkParams('name', 'UrlParam must be a string').isAlpha();
	req.checkBody('caption', 'BodyParam is not presented').notEmpty();
	if (!respObj.validateHttpParams(req, res)) {
		res.status(400).send(respObj.toJSonString());
		return;
	}

	// get entity name from urlParam
	var name = req.params.name;
	var caption = req.body.caption;

	ape.nbind.JsBindManager().getText(name, function(error, obj) {
		if (error) {
			respObj.addErrorItem({
				name: 'invalidCast',
				msg: obj,
				code: 666
			});
			res.status(400).send(respObj.toJSonString());
			return;
		}

		if (caption != obj.getCaption()) {
			respObj.addEvent({
				group: 'TEXT',
				type: 'TEXT_CAPTION',
				subjectName: obj.getName()
			});
		}
		obj.setCaption(caption);
		respObj.setData({
			caption: obj.getCaption()
		});
		res.send(respObj.toJSonString());
	});
});

module.exports = app;
