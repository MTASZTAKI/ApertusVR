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

exports.moduleTag = 'ApeHTTPApiNode';

app.get('/nodes', function(req, res) {
	var respObj = new resp(req);
	respObj.setDescription('Gets all node and their properties in the scene.');

	async.waterfall(
		[
			function(callback) {
				ape.nbind.JsBindManager().getUserNode(function(error, obj) {
					if (error) {
						callback({ name: 'invalidCast', msg: obj, code: 666 });
					}
					callback(null, obj.getName());
				});
			}
		],
		function(err, results) {
			if (err) {
				logger.error('error: ', err);
				respObj.addErrorItem(err);
				res.send(respObj.toJSonString());
				return;
			}

			respObj.addDataItem({ name: results });
			res.send(respObj.toJSonString());
		}
	);
});

app.post('/nodes', function(req, res) {
	var respObj = new resp(req);
	respObj.setDescription('Creates a new node with the specified name.');

	// handle http param validation errors
	req.checkBody('name', 'BodyParam is not presented').notEmpty()
	if (!respObj.validateHttpParams(req, res)) {
		res.status(400).send(respObj.toJSonString());
		return;
	}

	var name = req.body.name;
	var nodeObj = ape.nbind.JsBindManager().createNode(name);
	respObj.addEvent({
		group: 'NODE',
		type: 'NODE_CREATE',
		subjectName: nodeObj.getName()
	});
	res.send(respObj.toJSonString());
});

app.get('/nodes/:name/transformationmatrix', function(req, res) {
	var respObj = new resp(req);
	respObj.setDescription('Gets the homogen transformation matrix of the specified node.');

	// handle http param validation errors
	req.checkParams('name', 'UrlParam is not presented').notEmpty()
	if (!respObj.validateHttpParams(req, res)) {
		res.status(400).send(respObj.toJSonString());
		return;
	}

	var name = req.params.name;
	ape.nbind.JsBindManager().getNode(name, function(error, obj) {
		if (error) {
			respObj.addErrorItem({
				name: 'invalidCast',
				msg: obj,
				code: 666
			});
			res.status(400).send(respObj.toJSonString());
			return;
		}

		logger.debug(utils.convertToJsObj(obj));

		respObj.addDataItem({
			transformationmatrix: obj.getTransformationMatrix().toString()
		});
		res.send(respObj.toJSonString());
	});
});

app.get('/nodes/:name/scale', function(req, res) {
	var respObj = new resp(req);
	respObj.setDescription('Gets the scale of the specified node.');

	// handle http param validation errors
	req.checkParams('name', 'UrlParam is not presented').notEmpty()
	if (!respObj.validateHttpParams(req, res)) {
		res.status(400).send(respObj.toJSonString());
		return;
	}

	var name = req.params.name;
	ape.nbind.JsBindManager().getNode(name, function(error, obj) {
		if (error) {
			respObj.addErrorItem({
				name: 'invalidCast',
				msg: obj,
				code: 666
			});
			res.status(400).send(respObj.toJSonString());
			return;
		}

		var scaleObj = obj.getScale();
		var scale = {
			x: scaleObj.x,
			y: scaleObj.y,
			z: scaleObj.z
		};
		respObj.addDataItem({
			scale: scale
		});
		res.send(respObj.toJSonString());
	});
});

app.post('/nodes/:name/scale', function(req, res) {
	var respObj = new resp(req);
	respObj.setDescription('Sets the scale of the specified node.');

	// handle http param validation errors
	req.checkParams('name', 'UrlParam is not presented').notEmpty()
	req.checkBody('x', 'BodyParam is not presented').notEmpty();
	req.checkBody('y', 'BodyParam is not presented').notEmpty();
	req.checkBody('z', 'BodyParam is not presented').notEmpty();
	if (!respObj.validateHttpParams(req, res)) {
		res.status(400).send(respObj.toJSonString());
		return;
	}

	var name = req.params.name;
	ape.nbind.JsBindManager().getNode(name, function(error, obj) {
		if (error) {
			respObj.addErrorItem({
				name: 'invalidCast',
				msg: obj,
				code: 666
			});
			res.status(400).send(respObj.toJSonString());
			return;
		}

		var newScale = new ape.nbind.Vector3(Number(req.body.x),
										     Number(req.body.y),
										     Number(req.body.z));
		obj.setScale(newScale);
		respObj.addDataItem({
			position: utils.convertToJsObj(obj.getPosition())
		});
		res.send(respObj.toJSonString());
	});
});

app.get('/nodes/:name/position', function(req, res) {
	var respObj = new resp(req);
	respObj.setDescription('Gets the position of the specified node.');

	// handle http param validation errors
	req.checkParams('name', 'UrlParam is not presented').notEmpty()
	if (!respObj.validateHttpParams(req, res)) {
		res.status(400).send(respObj.toJSonString());
		return;
	}

	var name = req.params.name;
	ape.nbind.JsBindManager().getNode(name, function(error, obj) {
		if (error) {
			respObj.addErrorItem({
				name: 'invalidCast',
				msg: obj,
				code: 666
			});
			res.status(400).send(respObj.toJSonString());
			return;
		}

		respObj.addDataItem({
			position: utils.convertToJsObj(obj.getPosition())
		});
		res.send(respObj.toJSonString());
	});
});

app.post('/nodes/:name/position', function(req, res) {
	var respObj = new resp(req);
	respObj.setDescription('Sets the position of the specified node.');

	// handle http param validation errors
	req.checkParams('name', 'UrlParam is not presented').notEmpty()
	req.checkBody('x', 'BodyParam is not presented').notEmpty();
	req.checkBody('y', 'BodyParam is not presented').notEmpty();
	req.checkBody('z', 'BodyParam is not presented').notEmpty();
	if (!respObj.validateHttpParams(req, res)) {
		res.status(400).send(respObj.toJSonString());
		return;
	}

	var name = req.params.name;
	ape.nbind.JsBindManager().getNode(name, function(error, obj) {
		if (error) {
			respObj.addErrorItem({
				name: 'invalidCast',
				msg: obj,
				code: 666
			});
			res.status(400).send(respObj.toJSonString());
			return;
		}

		var newPos = new ape.nbind.Vector3(Number(req.body.x),
										   Number(req.body.y),
										   Number(req.body.z));
		obj.setPosition(newPos);
		respObj.addDataItem({
			position: utils.convertToJsObj(obj.getPosition())
		});
		res.send(respObj.toJSonString());
	});
});

app.get('/nodes/:name/orientation', function(req, res) {
	var respObj = new resp(req);
	respObj.setDescription('Gets the orientation of the specified node.');

	// handle http param validation errors
	req.checkParams('name', 'UrlParam is not presented').notEmpty()
	if (!respObj.validateHttpParams(req, res)) {
		res.status(400).send(respObj.toJSonString());
		return;
	}

	var name = req.params.name;
	ape.nbind.JsBindManager().getNode(name, function(error, obj) {
		if (error) {
			respObj.addErrorItem({
				name: 'invalidCast',
				msg: obj,
				code: 666
			});
			res.status(400).send(respObj.toJSonString());
			return;
		}

		respObj.addDataItem({
			orientation: utils.convertToJsObj(obj.getOrientation())
		});
		res.send(respObj.toJSonString());
	});
});

app.post('/nodes/:name/orientation', function(req, res) {
	var respObj = new resp(req);
	respObj.setDescription('Sets the orientation of the specified node.');

	// handle http param validation errors
	req.checkParams('name', 'UrlParam is not presented').notEmpty()
	req.checkBody('w', 'BodyParam is not presented').notEmpty();
	req.checkBody('x', 'BodyParam is not presented').notEmpty();
	req.checkBody('y', 'BodyParam is not presented').notEmpty();
	req.checkBody('z', 'BodyParam is not presented').notEmpty();
	if (!respObj.validateHttpParams(req, res)) {
		res.status(400).send(respObj.toJSonString());
		return;
	}

	var name = req.params.name;
	ape.nbind.JsBindManager().getNode(name, function(error, obj) {
		if (error) {
			respObj.addErrorItem({
				name: 'invalidCast',
				msg: obj,
				code: 666
			});
			res.status(400).send(respObj.toJSonString());
			return;
		}

		var newOrt = new ape.nbind.Quaternion(Number(req.body.w),
											  Number(req.body.x),
											  Number(req.body.y),
											  Number(req.body.z));
		obj.setOrientation(newOrt);
		respObj.addDataItem({
			orientation: utils.convertToJsObj(obj.getOrientation())
		});
		res.send(respObj.toJSonString());
	});
});

app.get('/nodes/:name/euler', function(req, res) {
	var respObj = new resp(req);
	respObj.setDescription('Gets the Euler (yaw, pitch, roll) of the specified node.');

	// handle http param validation errors
	req.checkParams('name', 'UrlParam is not presented').notEmpty()
	if (!respObj.validateHttpParams(req, res)) {
		res.status(400).send(respObj.toJSonString());
		return;
	}

	var name = req.params.name;
	ape.nbind.JsBindManager().getNode(name, function(error, obj) {
		if (error) {
			respObj.addErrorItem({
				name: 'invalidCast',
				msg: obj,
				code: 666
			});
			res.status(400).send(respObj.toJSonString());
			return;
		}

		var eulerObj = obj.getEuler();
		var eulerObjDegree = {
			y: Number(eulerObj.getYaw().toDegree()),
			p: Number(eulerObj.getPitch().toDegree()),
			r: Number(eulerObj.getRoll().toDegree()),
		}
		respObj.addDataItem({
			euler: eulerObjDegree
		});
		res.send(respObj.toJSonString());
	});
});

app.post('/nodes/:name/euler', function(req, res) {
	var respObj = new resp(req);
	respObj.setDescription('Sets the Euler (yaw, pitch, roll) of the specified node.');

	// handle http param validation errors
	req.checkParams('name', 'UrlParam is not presented').notEmpty()
	req.checkBody('y', 'BodyParam yaw is not presented').notEmpty();
	req.checkBody('p', 'BodyParam pitch is not presented').notEmpty();
	req.checkBody('r', 'BodyParam roll is not presented').notEmpty();
	if (!respObj.validateHttpParams(req, res)) {
		res.status(400).send(respObj.toJSonString());
		return;
	}

	var name = req.params.name;
	ape.nbind.JsBindManager().getNode(name, function(error, obj) {
		if (error) {
			respObj.addErrorItem({
				name: 'invalidCast',
				msg: obj,
				code: 666
			});
			res.status(400).send(respObj.toJSonString());
			return;
		}

		var newEuler = {
			y: Number(ape.nbind.Degree(Number(req.body.y)).toRadian()),
			p: Number(ape.nbind.Degree(Number(req.body.p)).toRadian()),
			r: Number(ape.nbind.Degree(Number(req.body.r)).toRadian())
		}
		obj.setEuler(ape.nbind.Euler(ape.nbind.Radian(ape.nbind.Degree(Number(req.body.y)).toRadian()),
									 ape.nbind.Radian(ape.nbind.Degree(Number(req.body.p)).toRadian()),
									 ape.nbind.Radian(ape.nbind.Degree(Number(req.body.r)).toRadian())));
		respObj.addDataItem({
			euler: newEuler
		});
		res.send(respObj.toJSonString());
	});
});

module.exports = app;
