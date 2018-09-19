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
var async = moduleManager.requireNodeModule('async');
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
				}
				else {
					if (item.properties.orientation) {
						var q = utils.quaternionFromAngleAxis(item.properties.orientation.angle, item.properties.orientation.axis);
						obj.setOrientation(q);
					}
				}
			});
		}
		else if (item.type === "gripper") {
			var mGripperLeftRootNodeInitialOrientation = ape.nbind.Quaternion(-0.99863, 0, 0, 0.0523337);
			var mGripperRightRootNodeInitialOrientation = ape.nbind.Quaternion(-0.99863, 0, 0, 0.0523337);
			var mGripperLeftHelperNodeInitialOrientation = ape.nbind.Quaternion(-0.418964, 0, 0, 0.908003);
			var mGripperRightHelperNodeInitialOrientation = ape.nbind.Quaternion(-0.418964, 0, 0, 0.908003);
			var mGripperLeftEndNodeInitialOrientation = ape.nbind.Quaternion(0.818923, 0, 0, 0.573904);
			var mGripperRightEndNodeInitialOrientation = ape.nbind.Quaternion(0.818923, 0, 0, 0.573904);

			var mGripperLeftRootOrientation = ape.nbind.Quaternion(1, 0, 0, 0);
			var mGripperRightRootOrientation = ape.nbind.Quaternion(1, 0, 0, 0);

			var gripperMaxValue = 255;
			var gripperMinValue = 0;
			var gripperCurrentValue = item.properties.value;
			var degreeStep = 45.0 / gripperMaxValue;
			var axis = ape.nbind.Vector3(0, 0, 1);
			var degree = ape.nbind.Degree(gripperCurrentValue * degreeStep)
			var orientation = ape.nbind.Quaternion();
			orientation.FromAngleAxis(ape.nbind.Radian(degree.toRadian()), axis);

			async.waterfall(
				[
					function (callback) {
						ape.nbind.JsBindManager().getNode('JOINT(Rotational)(gripR1)12ur10Gripper', function (error, obj) {
							if (error) {
								callback({ name: 'invalidCast', msg: obj, code: 666 });
							}
							else {
								mGripperLeftRootOrientation = mGripperLeftRootNodeInitialOrientation.product(orientation);
								obj.setOrientation(mGripperLeftRootOrientation);
								callback(null);
							}
						});
					},
					function (callback) {
						ape.nbind.JsBindManager().getNode('JOINT(Rotational)(gripR1)18ur10Gripper', function (error, obj) {
							if (error) {
								callback({ name: 'invalidCast', msg: obj, code: 666 });
							}
							else {
								mGripperRightRootOrientation = mGripperRightRootNodeInitialOrientation.product(orientation);
								obj.setOrientation(mGripperRightRootOrientation);
								callback(null);
							}
						});
					},
					function (callback) {
						ape.nbind.JsBindManager().getNode('JOINT(Rotational)(gripR5)16ur10Gripper', function (error, obj) {
							if (error) {
								callback({ name: 'invalidCast', msg: obj, code: 666 });
							}
							else {
								obj.setOrientation(mGripperLeftHelperNodeInitialOrientation.product(orientation));
								callback(null);
							}
						});
					},
					function (callback) {
						ape.nbind.JsBindManager().getNode('JOINT(Rotational)(gripR5)22ur10Gripper', function (error, obj) {
							if (error) {
								callback({ name: 'invalidCast', msg: obj, code: 666 });
							}
							else {
								obj.setOrientation(mGripperRightHelperNodeInitialOrientation.product(orientation));
								callback(null);
							}
						});
					},
					function (callback) {
						ape.nbind.JsBindManager().getNode('JOINT(Rotational)(gripR3)14ur10Gripper', function (error, obj) {
							if (error) {
								callback({ name: 'invalidCast', msg: obj, code: 666 });
							}
							else {
								obj.setOrientation(mGripperLeftEndNodeInitialOrientation.product(mGripperLeftRootOrientation.Inverse()));
								callback(null);
							}
						});
					},
					function (callback) {
						ape.nbind.JsBindManager().getNode('JOINT(Rotational)(gripR3)20ur10Gripper', function (error, obj) {
							if (error) {
								callback({ name: 'invalidCast', msg: obj, code: 666 });
							}
							else {
								obj.setOrientation(mGripperRightEndNodeInitialOrientation.product(mGripperRightRootOrientation.Inverse()));
								callback(null);
							}
						});
					}
				],
				function (err, results) {
					if (err) {
						logger.error('error: ', err);
						respObj.addErrorItem(err);
					}
				}
			);
		}
		else if (item.type == "text") {
			ape.nbind.JsBindManager().getText(item.name, function (error, obj) {
				if (error) {
					respObj.addErrorItem({
						name: 'invalidCast',
						msg: obj,
						code: 666
					});
				}
				else {
					if (item.properties && item.properties.caption) {
						obj.setCaption(item.properties.caption);
					}
				}
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
