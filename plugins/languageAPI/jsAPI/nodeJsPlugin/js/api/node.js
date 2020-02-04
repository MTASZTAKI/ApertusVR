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

app.get('/userNodeName', function(req, res) {
	var respObj = new resp(req);
	respObj.setDescription('Get the name of the user node and their properties in the scene.');

	ape.nbind.JsBindManager().getUserNode( function (error, obj) {
		if (error) {
			respObj.addErrorItem({
				name: 'invalidCast',
				msg: obj,
				code: 666
			});
			res.send(respObj.toJSonString());
		}
		else {
			respObj.addDataItem({ name: obj.getName() });
			res.send(respObj.toJSonString());
		}
	});
});

app.get('/nodeNames', function(req, res) {
	var respObj = new resp(req);
	respObj.setDescription('Gets all node names');

	async.waterfall(
		[
			function(callback) {
				ape.nbind.JsBindManager().getNodesNames(function(error, arr) {
					if (error) {
						callback({ name: 'invalidCast', msg: arr, code: 666 });
					}
					callback(null, arr);
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

			results.forEach(function (element) {
				respObj.addDataItem({ name: element });
			});

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

app.get('/calcTransform/:name1/:name2', function (req, res) {
	var respObj = new resp(req);
	respObj.setDescription('Calculates transformation matrix for node2 in node1 space');

	// handle http param validation errors
	req.checkParams('name1', 'UrlParam is not presented').notEmpty();
	req.checkParams('name2', 'UrlParam is not presented').notEmpty();
	if (!respObj.validateHttpParams(req, res)) {
		res.status(400).send(respObj.toJSonString());
		return;
	}

	var name1 = req.params.name1;
	logger.debug('name1: ', name1);
	var name2 = req.params.name2;
	logger.debug('name2: ', name2);

	var matrix1;
	var matrix2;

	async.waterfall(
		[
			function (callback) {
				ape.nbind.JsBindManager().getNode(name1, function (error, obj1) {
					if (error) {
						callback({ name: 'invalidCast', msg: obj1, code: 666 });
					}
					matrix1 = obj1.getTransformationMatrix();
					callback(null);
				});
			},
			function (callback) {
				ape.nbind.JsBindManager().getNode(name2, function (error, obj2) {
					if (error) {
						callback({ name: 'invalidCast', msg: obj2, code: 666 });
					}
					matrix2 = obj2.getTransformationMatrix();
					callback(null);
				});
			}
		],
		function (err, results) {
			if (err) {
				logger.error('error: ', err);
				respObj.addErrorItem(err);
				res.send(respObj.toJSonString());
				return;
			}

			var matrixRes = matrix2.concatenate(matrix1);
			//respObj.addDataItem(JSON.parse(matrixRes.toJsonString()));

			respObj.addDataItem({
				transformation: {
					invRotTransMatrix: JSON.parse(matrixRes.toJsonString())
				}
			});

			res.send(respObj.toJSonString());
		}
	);
});

app.get('/nodes/:name', function(req, res) {
	var respObj = new resp(req);
	respObj.setDescription('Gets all properties of the specified node.');

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

		var matrix = obj.getTransformationMatrix();
		var rotMatrix = new ape.nbind.Matrix4(-1, 0, 0, 0,
											  0, -1, 0, 0,
											  0, 0, 1, 0,
											  0, 0, 0, 1);
		var invRotTransMatrix = matrix.inverse().concatenate(rotMatrix);

		respObj.addDataItem({
			name: name,
			scale: JSON.parse(obj.getScale().toJsonString()),
			position: JSON.parse(obj.getPosition().toJsonString()),
			orientation: JSON.parse(obj.getOrientation().toJsonString()),
			euler: JSON.parse(obj.getEuler().toJsonString()),
			transformation: {
				transformationmatrix: JSON.parse(matrix.toJsonString()),
				invRotTransMatrix: JSON.parse(invRotTransMatrix.toJsonString())
			}
		});
		res.send(respObj.toJSonString());
	});
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
		var matrix = obj.getTransformationMatrix();
		var rotMatrix = new ape.nbind.Matrix4(-1, 0, 0, 0,
											  0, -1, 0, 0,
											  0, 0, 1, 0,
											  0, 0, 0, 1);
		var invRotTransMatrix = matrix.inverse().concatenate(rotMatrix);

		respObj.addDataItem({
			transformationmatrix: matrix.toString(),
			invRotTransMatrix: invRotTransMatrix.toString()
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

		respObj.addDataItem({
			scale: JSON.parse(obj.getScale().toJsonString())
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
			scale: JSON.parse(obj.getScale().toJsonString())
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
			position: JSON.parse(obj.getPosition().toJsonString())
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
			position: JSON.parse(obj.getPosition().toJsonString())
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
			orientation: JSON.parse(obj.getOrientation().toJsonString())
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
			orientation: JSON.parse(obj.getOrientation().toJsonString())
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

		respObj.addDataItem({
			euler: JSON.parse(obj.getEuler().toJsonString())
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
			euler: JSON.parse(obj.getEuler().toJsonString())
		});
		res.send(respObj.toJSonString());
	});
});

module.exports = app;
