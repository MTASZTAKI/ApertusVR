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
var x3d = require('../modules/utils/errors.js');;

app.get('/', function(req, res, next) {
	var respObj = new resp(req);
	respObj.setDescription('ApertusVR API Home');
	res.send(respObj.toJSonString());
});
app.post('/setfastai', function (req, res, next) {
	var respObj = new resp(req);
	respObj.setDescription('Sets properties on several objects at the same time.');

	if (!respObj.validateHttpParams(req, res)) {
		res.status(400).send(respObj.toJSonString());
		return;
	}
	if (!req.body.classes) {

		if (!req.body.x) {
			logger.error('no x array in http req body.data');
			var errObj = errorMap.items.dataNotPresented;
			errObj.message = 'Items array is not presented in Http Request body.data.';
			respObj.addErrorItem(errObj);
			res.send(respObj.toJSonString());
			return;
		}

		if (!req.body.y) {
			logger.error('no y array in http req body.data');
			var errObj = errorMap.items.dataNotPresented;
			errObj.message = 'Items array is not presented in Http Request body.data.';
			respObj.addErrorItem(errObj);
			res.send(respObj.toJSonString());
			return;
		}
		if (!req.body.z) {
			logger.error('no z array in http req body.data');
			var errObj = errorMap.items.dataNotPresented;
			errObj.message = 'Items array is not presented in Http Request body.data.';
			respObj.addErrorItem(errObj);
			res.send(respObj.toJSonString());
			return;
		}

		var respObj = new resp(req);
		for (var i = 0; i < req.body.x.length; i++) {
			var x_pos = 75*req.body.x[i];
			var y_pos = 120*req.body.y[i]+5;
			var z_pos = 65*req.body.z[i];
			console.log('x: ' + x_pos + ' y: ' + y_pos + ' z: ' + z_pos);
			ape.nbind.JsBindManager().getNode('dsa'+i, function (error, obj) {
				if (error) {
					respObj.addErrorItem({
						name: 'invalidCast',
						msg: obj,
						code: 666
					});
				}
				else {
						obj.setPosition(ape.nbind.Vector3(x_pos, y_pos, z_pos));
				}
			});
			ape.nbind.JsBindManager().getNode('aaa' + i, function (error, obj) {
				if (error) {
					respObj.addErrorItem({
						name: 'invalidCast',
						msg: obj,
						code: 666
					});
				}
				else {
					obj.setPosition(ape.nbind.Vector3(x_pos, y_pos+2, z_pos));
				}
			});
		}
	}
	else {
		var classes = req.body.classes; //number of classes

		var parentBox = ape.nbind.JsBindManager().createNode('xBox', true);
		parentBox.setPosition(ape.nbind.Vector3(-20, 0, 10));
		var boxSetObj = ape.nbind.JsBindManager().createBox('xAxis', true);
		var dimensionsAtrr = ape.nbind.Vector3(0.2, 200, 0.2);
		boxSetObj.setParameters(dimensionsAtrr);
		boxSetObj.setParentNodeJsPtr(parentBox);
		var xMat = ape.nbind.JsBindManager().createManualMaterial("xmat", true);
		var diffuseColor = ape.nbind.Color(1, 0, 0, 1);			
		xMat.setDiffuseColor(diffuseColor);
		boxSetObj.setManualMaterial(xMat);

		var parentNode1 = ape.nbind.JsBindManager().createNode('xtext', true);
		parentNode1.setPosition(ape.nbind.Vector3(-20, 100, 10));
		var textGeometry = ape.nbind.JsBindManager().createText("Loss", true);
		textGeometry.setParentNodeJsPtr(parentNode1);
		
		var parentBox2 = ape.nbind.JsBindManager().createNode('yBox', true);
		parentBox2.setPosition(ape.nbind.Vector3(-20, 2, 10));
		var boxSetObj = ape.nbind.JsBindManager().createBox('yAxis', true);
		var dimensionsAtrr = ape.nbind.Vector3(0.2, 0.2, 200);
		boxSetObj.setParameters(dimensionsAtrr);
		boxSetObj.setParentNodeJsPtr(parentBox2);
		var yMat = ape.nbind.JsBindManager().createManualMaterial("ymat", true);
		var diffuseColor = ape.nbind.Color(0, 0, 1, 1);
		yMat.setDiffuseColor(diffuseColor);
		boxSetObj.setManualMaterial(yMat);

		var parentNode1 = ape.nbind.JsBindManager().createNode('ytext1', true);
		parentNode1.setPosition(ape.nbind.Vector3(-20, 2, 95));
		var textGeometry = ape.nbind.JsBindManager().createText("Cat Prediction", true);
		textGeometry.setParentNodeJsPtr(parentNode1);

		var parentNode1 = ape.nbind.JsBindManager().createNode('ytext2', true);
		parentNode1.setPosition(ape.nbind.Vector3(-20, 2, -75));
		var textGeometry = ape.nbind.JsBindManager().createText("Dog Prediction", true);
		textGeometry.setParentNodeJsPtr(parentNode1);


		for (var i = 0; i < req.body.IDs.length; i++) {
			var image_path = req.body.IDs[i]; // paths to the validation set images
			var parentNode = ape.nbind.JsBindManager().createNode('dsa'+i, true);
			parentNode.setPosition(ape.nbind.Vector3(10, 30, 10));
			//parentNode.setOrientation(new ape.nbind.Quaternion(0.7071, -0.7071, 0, 0));

			var manualMaterial = ape.nbind.JsBindManager().createManualMaterial("sda" + i, true);
			var fileTexture = ape.nbind.JsBindManager().createFileTexture(image_path, true);
			fileTexture.setFileName(image_path);
			manualMaterial.setTexture(fileTexture);

			//var parentNode1 = ape.nbind.JsBindManager().createNode('aaa' + i);
			//parentNode1.setPosition(ape.nbind.Vector3(10, 30, 10));	
			//var text = "";
			//if (image_path[image_path.length - 5] == 1)
			//	text = "Neoplastic";
			//else text = "NotNeoplastic";
			//var textGeometry = ape.nbind.JsBindManager().createText(text + i);
			//textGeometry.setParentNodeJsPtr(parentNode1);
			//textGeometry.setCaption(text);

			var plane = ape.nbind.JsBindManager().createPlane('das' + i, true);
			var numSeg = ape.nbind.Vector2(2, 2);
			var size = ape.nbind.Vector2(10, 10);
			var tile = ape.nbind.Vector2(1, 1);
			plane.setParameters(numSeg, size, tile);
			plane.setParentNodeJsPtr(parentNode);
			plane.setManualMaterial(manualMaterial);
		}
		
		
		//var matItem = currentItem.siblings('Appearance').first().children('Material').first();
		//self.parseMaterial(matItem, boxSetObj);

		/*if (parentNodeObj) {
			boxSetObj.setParentNodeJsPtr(parentNodeObj);
			log(' - this: ' + boxSetObj.getName() + ' - parentNode: ' + parentNodeObj.getName());
		}*/

	}
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
