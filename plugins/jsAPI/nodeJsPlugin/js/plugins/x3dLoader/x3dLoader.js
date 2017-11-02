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

var utils = require('../../modules/utils/utils.js');
var ape = require('../../ape.js');
var moduleManager = require('../../modules/module_manager/module_manager.js');
var cheerio = moduleManager.requireNodeModule('cheerio');
var htmlparser = moduleManager.requireNodeModule('htmlparser2');
var fs = require('fs');
var async = moduleManager.requireNodeModule('async');
const uuidV1 = moduleManager.requireNodeModule('uuid/v1');
const path = require('path');
var request = moduleManager.requireNodeModule('request');
var self = this;
var nameCounter = 0;

function log(...args) {
	// console.log(args);
}

function splitX3DAttr(str) {
	var res = str;
	res = res.replace(/[\r\n]+/g, '');
	res = res.replace(/ +(?= )/g, '');
	res = res.trim();
	res = res.split(' ');
	log('--------------------------------------------------------------------------------');
	log('res: ' + res);
	return res;
	//return str.replace(/[\r\n]+/g, '').replace(/ +(?= )/g,'').trim().split(' ');
}

exports.parseCoordIndexAttr = function(currentItem) {
	if (!utils.isDefined(currentItem)) {
		throw 'currentItem is not defined!';
		return null;
	}

	var coordinates = new Array();
	var erdosCoordinates = new Array();
	var coordIndex = currentItem.attr('coordIndex');

	if (!utils.isDefined(coordIndex)) {
		throw 'coordIndex is not defined!';
		return null;
	}

	var itemsArr = splitX3DAttr(coordIndex);

	//@lineset it is valid
	/*if (itemsArr.length < 4) {
	  throw 'itemsArr length is less than 4!';
	  return null;
	}*/

	for (var i = 0; i < itemsArr.length; i++) {
		coordinates.push(itemsArr[i]);
	}

	var tempArr = new Array();
	var indexCount = 0;
	for (var i = 0; i < coordinates.length; i++) {
		if (coordinates[i] == -1) {
			// erdos-specific mirroring
			if (indexCount == 5) {
				tempArr.splice(0, 1);
				tempArr.reverse();
			}
			erdosCoordinates.pushArray(tempArr);
			erdosCoordinates.push(-1);

			// create opposite face for the other side
			// if (indexCount == 5) {
			//   tempArr.reverse();
			//   erdosCoordinates.pushArray(tempArr);
			//   erdosCoordinates.push(-1);
			// }
			tempArr = new Array();
			indexCount = 0;
		} else {
			tempArr.push(coordinates[i]);
		}
		indexCount++;
	}

	log(' - coordIndex: ' + erdosCoordinates);
	return erdosCoordinates;
}

exports.parseColorIndexAttr = function(currentItem) {
	var colorIndices = new Array();
	var colorIndex = currentItem.attr('colorIndex');
	if (utils.isDefined(colorIndex)) {
		var itemsArr = splitX3DAttr(colorIndex);
		for (var i = 0; i < itemsArr.length; i++) {
			colorIndices.push(Number(itemsArr[i]));
		}
	}
	return colorIndices;
}

exports.parseCoordinatePointAttr = function(currentItem) {
	if (!utils.isDefined(currentItem)) {
		throw 'currentItem is not defined!';
		return null;
	}

	var pointsArr = new Array();
	var coordinate = currentItem.find('Coordinate').first();

	if (!utils.isDefined(coordinate)) {
		throw 'coordinate attrib is not defined!';
		return null;
	}

	var pointAttr = coordinate.attr('point');
	if (!utils.isDefined(pointAttr)) {
		throw 'point attrib is not defined!';
		return null;
	}

	var itemsArr = splitX3DAttr(pointAttr);
	if (itemsArr.length == 0) {
		throw 'pointAttr length is 0!';
		return null;
	}

	for (var j = 0; j < itemsArr.length; j++) {
		pointsArr.push(Number(itemsArr[j]));
	}

	log(' - coordinatePoints: ' + pointsArr);
	return pointsArr;
}

exports.parseNormals = function(currentItem) {
	var normals = new Array();

	if (!utils.isDefined(currentItem)) {
		log(' currentItem is not defined ');
		return normals;
	}

	var normal = currentItem.find('Normal').first();

	if (!utils.isDefined(normal)) {
		log(' Normal was not defined ');
		return normals;
	}

	if (normal.length == 0) {
		log(' Normal was not defined2 ');
		return normals;
	}

	var normalAttr = normal.attr('vector');
	if (!utils.isDefined(normalAttr)) {
		log(' Normal vector was not defined ');
		return normals;
	}

	var itemsArr = splitX3DAttr(normalAttr);
	if (itemsArr.length == 0) {
		log(' Normal vector length is 0 ');
		return normals;
	}

	for (var j = 0; j < itemsArr.length; j++) {
		normals.push(Number(itemsArr[j]));
	}

	log(' - Normals: ' + normals);
	return normals;
}

exports.parseColorRGBAs = function(currentItem) {
	var colorRGBAs = new Array();
	if (!utils.isDefined(currentItem)) {
		//throw 'currentItem is not defined!';
		return colorRGBAs;
	}
	var colorRGBA = currentItem.find('ColorRGBA').first();

	if (!utils.isDefined(colorRGBA)) {
		log(' ColorRGBA was not defined ');
		return colorRGBAs;
	}

	if (colorRGBA.length == 0) {
		log(' ColorRGBA was not defined2 ');
		return colorRGBAs;
	}

	var colorAttr = colorRGBA.attr('color');
	if (!utils.isDefined(colorAttr)) {
		log(' ColorRGBA color was not defined ');
		return colorRGBAs;
	}

	var itemsArr = splitX3DAttr(colorAttr);
	if (itemsArr.length == 0) {
		log(' ColorRGBA color length is 0 ');
		return colorRGBAs;
	}

	for (var j = 0; j < itemsArr.length; j++) {
		colorRGBAs.push(Number(itemsArr[j]));
	}

	log(' - ColorRGBA: ' + colorRGBAs);
	return colorRGBAs;
}

exports.parsePositionInterpolatorKeyValuesAttr = function(currentItem) {
	if (!utils.isDefined(currentItem)) {
		throw 'currentItem is not defined!';
		return null;
	}

	var items = new Array();

	var keyValueArr = currentItem.attr('keyValue');
	if (!utils.isDefined(keyValueArr)) {
		throw 'keyValue attrib is not defined!';
		return null;
	}

	var itemsArr = splitX3DAttr(keyValueArr);
	if (itemsArr.length == 0) {
		throw 'pointAttr length is 0!';
		return null;
	}

	for (var j = 0; j < itemsArr.length; j = j + 3) {
		items.push(new ape.nbind.Vector3(Number(itemsArr[j]), Number(itemsArr[j + 1]), Number(itemsArr[j + 2])));
	}

	log(' - keyValue: ' + items);
	return items;
}

exports.parseOrientationInterpolatorKeyValuesAttr = function(currentItem) {
	if (!utils.isDefined(currentItem)) {
		throw 'currentItem is not defined!';
		return null;
	}

	var items = new Array();

	var keyValueArr = currentItem.attr('keyValue');
	if (!utils.isDefined(keyValueArr)) {
		throw 'keyValue attrib is not defined!';
		return null;
	}

	var itemsArr = splitX3DAttr(keyValueArr);
	if (itemsArr.length == 0) {
		throw 'pointAttr length is 0!';
		return null;
	}

	for (var j = 0; j < itemsArr.length; j = j + 4) {
		var axis = ape.nbind.Vector3(Number(itemsArr[j]), Number(itemsArr[j + 1]), Number(itemsArr[j + 2]));
		var radian = ape.nbind.Radian(Number(itemsArr[j + 3]));
		items.push(new ape.nbind.Quaternion(radian, axis));
	}

	log(' - keyValue: ' + items);
	return items;
}

exports.parseTranslationAttr = function(currentItem) {
	var translation = currentItem.attr('translation');
	if (utils.isDefined(translation)) {
		log(' - translation: ' + translation);
		var itemArr = splitX3DAttr(translation);
		if (itemArr.length == 3) {
			return new ape.nbind.Vector3(Number(itemArr[0]), Number(itemArr[1]), Number(itemArr[2]));
		}
	}
	return new ape.nbind.Vector3();
}

exports.parseRotationAttr = function(currentItem) {
	var rotation = currentItem.attr('rotation');
	if (utils.isDefined(rotation)) {
		log(' - rotation: ' + rotation);
		var itemArr = splitX3DAttr(rotation);
		if (itemArr.length == 4) {
			var axis = ape.nbind.Vector3(Number(itemArr[0]), Number(itemArr[1]), Number(itemArr[2]));
			var radian = ape.nbind.Radian(Number(itemArr[3]));
			return new ape.nbind.Quaternion(radian, axis);
		}
	}
	return new ape.nbind.Quaternion();
}

exports.parseScaleAttr = function(currentItem) {
	var scale = currentItem.attr('scale');
	if (utils.isDefined(scale)) {
		log(' - scale: ' + scale);
		var itemArr = splitX3DAttr(scale);
		if (itemArr.length == 3) {
			return new ape.nbind.Vector3(Number(itemArr[0]), Number(itemArr[1]), Number(itemArr[2]));
		}
	}
	return new ape.nbind.Vector3(1, 1, 1);
}

exports.parseTransparencyAttr = function(currentItem) {
	var transparency = currentItem.attr('transparency');
	if (utils.isDefined(transparency)) {
		log(' - transparency: ' + transparency);
		return Number(transparency);
	}
	return 0.0;
}

exports.parseDiffuseColorAttr = function(currentItem, transparency) {
	var diffuseColor = currentItem.attr('diffuseColor');
	if (utils.isDefined(diffuseColor)) {
		log(' - diffuseColor: ' + diffuseColor);
		var itemArr = splitX3DAttr(diffuseColor);
		if (itemArr.length == 3) {
			return new ape.nbind.Color(Number(itemArr[0]), Number(itemArr[1]), Number(itemArr[2]), 1 - transparency);
		}
	}
	return new ape.nbind.Color(1, 1, 1, 1);
}

exports.parseSpecularColorAttr = function(currentItem, transparency) {
	var specularColor = currentItem.attr('specularColor');
	if (utils.isDefined(specularColor)) {
		log(' - specularColor: ' + specularColor);
		var itemArr = splitX3DAttr(specularColor);
		if (itemArr.length == 3) {
			return new ape.nbind.Color(Number(itemArr[0]), Number(itemArr[1]), Number(itemArr[2]), 1 - transparency);
		}
	}
	return new ape.nbind.Color(1, 1, 1, 1);
}

exports.parseMaterial = function(matItem, parentGeometry) {
	if (matItem && matItem[0]) {
		var itemName = matItem[0].itemName || parentGeometry.getName() + 'material';
		log('itemName: ' + itemName);
		var manualMaterial = ape.nbind.JsBindManager().createManualMaterial(itemName);
		//var manualPass = ape.nbind.JsBindManager().createManualPass(itemName + 'ManualPass');
		var transparency = self.parseTransparencyAttr(matItem);
		var diffuseColor = self.parseDiffuseColorAttr(matItem, transparency);
		var specularColor = self.parseSpecularColorAttr(matItem, transparency);
		manualMaterial.setDiffuseColor(diffuseColor);
		manualMaterial.setSpecularColor(specularColor);
		//manualPass.setDiffuseColor(diffuseColor);
		//manualPass.setSpecularColor(specularColor);
		//manualMaterial.setManualPass(manualPass);
		return manualMaterial;
	}


	log('parseMaterial return null');
	return null;
}

exports.parseColorAttr = function(currentItem) {
	var color = currentItem.find('color').first();
	if (!utils.isDefined(color)) {
		throw 'color attrib is not defined!';
		return null;
	}
	var colorAttr = color.attr('color');
	if (utils.isDefined(colorAttr)) {
		log(' - color: ' + colorAttr);
		var itemArr = splitX3DAttr(colorAttr);
		if (itemArr.length == 3) {
			return new ape.nbind.Color(Number(itemArr[0]), Number(itemArr[1]), Number(itemArr[2]), 1);
		}
	}
	return new ape.nbind.Color(1, 1, 1, 1);
}

exports.parseDimensionsAttr = function(currentItem) {
	var dimensions = currentItem.attr('size');
	if (utils.isDefined(dimensions)) {
		log(' - dimensions: ' + dimensions);
		var itemArr = splitX3DAttr(dimensions);
		if (itemArr.length == 3) {
			return new ape.nbind.Vector3(Number(itemArr[0]), Number(itemArr[1]), Number(itemArr[2]));
		}
	}
	return new ape.nbind.Vector3(1, 1, 1);
}

// ---

exports.parseItem = function(parentItem, currentItem, parentNodeObj) {
	if (!utils.isDefined(currentItem) && !utils.isDefined(currentItem[0]))
		return false;

	var typeName = currentItem[0].type;
	var tagName = currentItem[0].tagName || currentItem[0].name;
	var itemName = ((currentItem[0].hasOwnProperty('itemName') && currentItem[0].itemName) || currentItem.attr('DEF') || 'item' + nameCounter);
	nameCounter++;
	itemName = itemName + currentlyLoadingFileName;
	currentItem[0].itemName = itemName
	log(itemName + ' - ' + typeName + ' - ' + tagName);

	if (!typeName || !tagName) {
		log('currentItem has no typeName or tagName, returning.');
		return true; // continue;
	}

	if (typeName == 'tag') {
		if (tagName == '?xml') {
			//
		} else if (tagName == 'x3d') {
			//
		} else if (tagName == 'meta') {
			var name = currentItem.attr('name');
			if (utils.isDefined(name)) {
				log(' - name: ' + name);
				//  TODO: create a textGeometry
			}

			var content = currentItem.attr('content');
			if (utils.isDefined(content)) {
				log(' - content: ' + content);
				//  TODO: create a textGeometry
			}
		} else if (tagName == 'scene') {
			var nodeObj = ape.nbind.JsBindManager().createNode(currentlyLoadingFileName);
			nodeLevel++;
			if (currentlyLoadingFileName == 'weldingFixture') {
				nodeObj.setScale(new ape.nbind.Vector3(0.1, 0.1, 0.1));
				nodeObj.setOrientation(new ape.nbind.Quaternion(0.7071, -0.7071, 0, 0));
				nodeObj.setPosition(new ape.nbind.Vector3(0, 0, 100000));
			}
			if (currentlyLoadingFileName == 'cell') {
				nodeObj.setScale(new ape.nbind.Vector3(0.1, 0.1, 0.1));
				nodeObj.setPosition(new ape.nbind.Vector3(0, 0, 0));
				nodeObj.setOrientation(new ape.nbind.Quaternion(0.7071, -0.7071, 0, 0));
			}
			if (currentlyLoadingFileName == 'ur5cellAnim') {
				nodeObj.setScale(new ape.nbind.Vector3(0.1, 0.1, 0.1));
				nodeObj.setPosition(new ape.nbind.Vector3(151, -78, -185));
				nodeObj.setOrientation(new ape.nbind.Quaternion(0.5, -0.5, -0.5, -0.5));
			}
			if (currentlyLoadingFileName == 'TwoUR5') {
				nodeObj.setScale(new ape.nbind.Vector3(0.1, 0.1, 0.1));
				nodeObj.setPosition(new ape.nbind.Vector3(151, -78, -185));
				nodeObj.setOrientation(new ape.nbind.Quaternion(0.5, -0.5, -0.5, -0.5));
			}
			if (currentlyLoadingFileName == 'SuperChargerLinkage') {
				nodeObj.setScale(new ape.nbind.Vector3(0.1, 0.1, 0.1));
				nodeObj.setOrientation(new ape.nbind.Quaternion(0.7071, -0.7071, 0, 0));
				nodeObj.setPosition(new ape.nbind.Vector3(0, 0, 200000));
			}
			if (currentlyLoadingFileName == 'stand') {
				log(' scene world node created for: ' + currentlyLoadingFileName);
				nodeObj.setScale(new ape.nbind.Vector3(100, 100, 100));
				nodeObj.setPosition(new ape.nbind.Vector3(-12000, -200, -3000));
				nodeObj.setOrientation(new ape.nbind.Quaternion(0.7071, -0.7071, 0, 0));
			}
			if (currentlyLoadingFileName == 'SZTAKIUr5Cell') {
				log(' scene world node created for: ' + currentlyLoadingFileName);
				nodeObj.setScale(new ape.nbind.Vector3(0.1, 0.1, 0.1));
				nodeObj.setPosition(new ape.nbind.Vector3(-140, -110, 50));
				nodeObj.setOrientation(new ape.nbind.Quaternion(0.5, -0.5, 0.5, 0.5));
			}
			if (currentlyLoadingFileName == 'PersonalComputer') {
				nodeObj.setScale(new ape.nbind.Vector3(0.1, 0.1, 0.1));
				nodeObj.setPosition(new ape.nbind.Vector3(-50, 0, 50));
				nodeObj.setOrientation(new ape.nbind.Quaternion(0.5, -0.5, -0.5, -0.5));
			}
			return nodeObj;
		} else if (tagName == 'worldinfo') {
			var info = currentItem.attr('info');
			if (utils.isDefined(info)) {
				log(' - info: ' + info);
				//  TODO: create a textGeometry
			}
		} else if (tagName == 'viewpoint') {
			//
		} else if (tagName == 'shape') {
			var use = currentItem.attr('USE');
			if (utils.isDefined(use)) {
				var geometryName = use + currentlyLoadingFileName;
				var fileGeometryObj = ape.nbind.JsBindManager().createFileGeometry(itemName);
				fileGeometryObj.setFileName(geometryName);
				log('USE: ' + fileGeometryObj.getName());

				if (parentNodeObj) {
					fileGeometryObj.setParentNodeJsPtr(parentNodeObj);
					log(' - this: ' + fileGeometryObj.getName() + ' - parentNode: ' + parentNodeObj.getName());
				}
			}
		} else if (tagName == 'indexedfaceset') {
			var grouped = false;
			var groupNodeObjName = itemName;
			if (groupNodeObj) {
				groupNodeObjName = groupNodeObj.getName();
				grouped = true;
			}
			log('- indexedfaceset:' + groupNodeObjName);
			var HANDLING = groupNodeObjName.indexOf("handling");
			var FIXTURE = groupNodeObjName.indexOf("WeldingFixture@p");
			if (HANDLING < 0 && FIXTURE < 0) {
				var indexedFaceSetObj = ape.nbind.JsBindManager().createIndexedFaceSet(itemName);
				var coordinatePointsArr = self.parseCoordinatePointAttr(currentItem);
				var coordIndexArr = self.parseCoordIndexAttr(currentItem);
				var normals = self.parseNormals(currentItem);
				var colors = self.parseColorRGBAs(currentItem);
				var matItem = currentItem.siblings('Appearance').first().children('Material').first();
				var materialObj = self.parseMaterial(matItem, indexedFaceSetObj);
				if (utils.isDefined(materialObj)) {
					log('setParametersWithMaterial is called');
					indexedFaceSetObj.setParametersWithMaterial(groupNodeObjName, coordinatePointsArr, coordIndexArr, normals, colors, materialObj);
				} else
					indexedFaceSetObj.setParameters(groupNodeObjName, coordinatePointsArr, coordIndexArr, normals, colors);
				if (lastGroupNodeObjName != groupNodeObjName) {
					if (groupNodeObj) {
						indexedFaceSetObj.setParentNodeJsPtr(groupNodeObj);
						log('- groupNodeObj:' + groupNodeObjName);
					}
					lastGroupNodeObjName = groupNodeObjName;
				}
				if (!grouped) {
					if (parentNodeObj) {
						indexedFaceSetObj.setParentNodeJsPtr(parentNodeObj);
						log(' - this: ' + indexedFaceSetObj.getName() + ' - parentNode: ' + parentNodeObj.getName());
					} else
						log('no parent, no group -> no node to attach the geometry');
				}
			}
		} else if (tagName == 'box') {
			var boxSetObj = ape.nbind.JsBindManager().createBox(itemName);
			var dimensionsAtrr = self.parseDimensionsAttr(currentItem);
			boxSetObj.setParameters(dimensionsAtrr);
			var matItem = currentItem.siblings('Appearance').first().children('Material').first();
			self.parseMaterial(matItem, boxSetObj);

			if (parentNodeObj) {
				boxSetObj.setParentNodeJsPtr(parentNodeObj);
				log(' - this: ' + boxSetObj.getName() + ' - parentNode: ' + parentNodeObj.getName());
			}
		}
		/*else if (tagName == 'indexedlineset') {
		    var indexedLineSetObj = ape.nbind.JsBindManager().createIndexedLineSet(itemName);
		    var coordinatePointsArr = self.parseCoordinatePointAttr(currentItem);
		    var coordIndexArr = self.parseCoordIndexAttr(currentItem);
		    var color = self.parseColorAttr(currentItem);
		    indexedLineSetObj.setParameters(coordinatePointsArr, coordIndexArr, color);

		    if (parentNodeObj) {
		        indexedLineSetObj.setParentNodeJsPtr(parentNodeObj);
		        log(' - this: ' + indexedLineSetObj.getName() + ' - parentNode: ' + parentNodeObj.getName());
		    }
		}*/
		else if (tagName == 'transform') {
			var nodeObj = ape.nbind.JsBindManager().createNode(itemName);
			nodeLevel++;

			var position = self.parseTranslationAttr(currentItem);
			nodeObj.setPosition(position);
			var orientation = self.parseRotationAttr(currentItem);
			nodeObj.setOrientation(orientation);
			var scale = self.parseScaleAttr(currentItem);
			nodeObj.setScale(scale);

			if (parentNodeObj) {
				nodeObj.setParentNodeJsPtr(parentNodeObj);
				log(' - this: ' + nodeObj.getName() + ' - parentNode: ' + parentNodeObj.getName());
			}

			return nodeObj;
		} else if (tagName == 'group') {
			var nodeObj = ape.nbind.JsBindManager().createNode(itemName);
			nodeLevel++;

			if (parentNodeObj) {
				nodeObj.setParentNodeJsPtr(parentNodeObj);
				log(' - this: ' + nodeObj.getName() + ' - parentNode: ' + parentNodeObj.getName());
			}
			groupNodeObj = nodeObj;
			log('groupChanged: ' + groupNodeObj.getName());
			return nodeObj;
		} else if (tagName == 'switch') {
			var nodeObj = ape.nbind.JsBindManager().createNode(itemName);
			nodeLevel++;

			if (parentNodeObj) {
				nodeObj.setParentNodeJsPtr(parentNodeObj);
				log(' - this: ' + nodeObj.getName() + ' - parentNode: ' + parentNodeObj.getName());
			}
			return nodeObj;
		} else if (tagName == 'orientationinterpolator') {
			var orientationInterpolator = {
				type: 'orientation',
				name: itemName,
				keys: new Array(),
				keyValues: new Array(),
				nodeName: '',
				nodeObj: 0
			};
			var keys = splitX3DAttr(currentItem.attr('key'));
			var keyValues = self.parseOrientationInterpolatorKeyValuesAttr(currentItem);
			orientationInterpolator.keys = keys;
			orientationInterpolator.keyValues = keyValues;
			interpolatorArr.push(orientationInterpolator);
			log('OrientationInterpolator: ' + itemName);
		} else if (tagName == 'positioninterpolator') {
			var positionInterpolator = {
				type: 'position',
				name: itemName,
				keys: new Array(),
				keyValues: new Array(),
				nodeName: '',
				nodeObj: 0
			};
			var keys = splitX3DAttr(currentItem.attr('key'));
			var keyValues = self.parsePositionInterpolatorKeyValuesAttr(currentItem);
			positionInterpolator.keys = keys;
			positionInterpolator.keyValues = keyValues;
			interpolatorArr.push(positionInterpolator);
			log('PositionInterpolator: ' + itemName);
		} else if (tagName == 'route') {
			var interpolatorName = currentItem.attr('fromNode') + currentlyLoadingFileName;
			var nodeName = currentItem.attr('toNode') + currentlyLoadingFileName;
			log('ROUTE: ' + interpolatorName);
			for (var i = 0; i < interpolatorArr.length; i++) {
				if (interpolatorArr[i].name == interpolatorName) {
					interpolatorArr[i].nodeName = nodeName;
					ape.nbind.JsBindManager().getNode(interpolatorArr[i].nodeName, function(error, object) {
						if (error) {
							log(error);
							return;
						}
						interpolatorArr[i].nodeObj = object;
					});
				}
			}
		} else if (tagName == 'timesensor') {
			loopAnimation = currentItem.attr('loop');
			cycleIntervalAnimation = Number(currentItem.attr('cycleInterval'));
		} else {

		}
	}
}


var nodeLevelMap = {};
var nodeLevel = 0;
var nodeLevelTmp = 0;
var lastGroupNodeObjName = '';
var groupNodeObj = 0;
var interpolatorArr = new Array();
var loopAnimation = false;
var cycleIntervalAnimation = 1;
var keyIndex = 0;
var currentlyLoadingFileName = '';
var config;

exports.parseTree = function($, parentItem, childItem, parentNodeObj) {
	if (!childItem) {
		return;
	}

	var currentNode;
	try {
		currentNode = self.parseItem(parentItem, childItem, parentNodeObj);
	} catch (e) {
		log('Exception cached: ' + e);
		return;
	}

	if (currentNode) {
		nodeLevelMap[nodeLevel] = currentNode;
	}
	nodeLevelTmp = nodeLevel;

	$(childItem).children().each(function(i, elem) {
		var currentChild = $(this);
		self.parseTree($, $(currentChild).parent(), $(currentChild), nodeLevelMap[nodeLevel]);
	});

	if (currentNode) {
		nodeLevel = nodeLevel - 1;
	}

	return;
}

exports.parseX3DSync = function(x3dFilePath) {
	var $ = cheerio.load(fs.readFileSync(x3dFilePath), {
		xmlMode: true,
		lowerCaseTags: true
	});
	self.parseTree($, null, $('X3D'), null);
}

exports.parseX3DAsync = function(x3dFilePath, callback) {
	var source = fs.createReadStream(x3dFilePath);

	var options = {
		xmlMode: true,
		lowerCaseTags: true
	};

	var cheerioStream = htmlparser.createDomStream(function done(err, dom) {
		if (err) {
			cheerioStream.emit('error', err);
		} else {
			cheerioStream.emit('finish', cheerio.load(dom));
		}
	}, options);
	cheerioStream.on('finish', ($) => {
		log('finish');
		log($);

		self.parseTree($, null, $('X3D'), null);
		callback();
	});

	source.pipe(cheerioStream);
}

exports.Animate = function() {

	function DoInterpolationSteps() {
		//log('keyIndex:' + keyIndex);
		for (var interpolatorID = 0; interpolatorID < interpolatorArr.length; interpolatorID++) {
			if (keyIndex < interpolatorArr[interpolatorID].keyValues.length) {
				if (interpolatorArr[interpolatorID].type == 'position') {
					interpolatorArr[interpolatorID].nodeObj.setPosition(interpolatorArr[interpolatorID].keyValues[keyIndex]);
				} else if (interpolatorArr[interpolatorID].type == 'orientation') {
					interpolatorArr[interpolatorID].nodeObj.setOrientation(interpolatorArr[interpolatorID].keyValues[keyIndex]);
				}
			}
		}
		keyIndex++;
	}
	log('X3D-animation play begin:');

	for (var i = 0; i < 200; i++) {
		setTimeout(function() {
			DoInterpolationSteps();
		}, 25 * i);
		//log('timed out for:' + 25 * i);
	}

	log('X3D-animation play end:');
}

exports.resetGlobalValues = function() {
	nodeLevelMap = {};
	nodeLevel = 0;
	nodeLevelTmp = 0;
	lastGroupNodeObjName = '';
	groupNodeObj = 0;
}

exports.loadFiles = function() {
	log("X3DLoaderPlugin.loadFiles()");

	var configFolderPath = ape.nbind.JsBindManager().getFolderPath();
	log('X3DLoaderPlugin configFolderPath: ' + configFolderPath);

	config = require(configFolderPath + '\\ApeX3DLoaderPlugin.json');
	log('X3DLoaderPlugin files to load: ', config.files);
	config.files = config.files.reverse();

	asyncFunctions = new Array();
	for (var i = 0; i < config.files.length; i++) {
		var fn = function(callback) {
			var filePath = moduleManager.sourcePath + config.files.pop();
			currentlyLoadingFileName = filePath.substring(filePath.lastIndexOf('/') + 1, filePath.lastIndexOf('.'));
			self.parseX3DAsync(filePath, function() {
				log('X3D-parsing done: ' + filePath + ' with name: ' + currentlyLoadingFileName);
				callback(null);
			});
		}
		asyncFunctions.push(fn);
	}

	async.waterfall(
			asyncFunctions,
			function(err, result) {
				log("async tasks done");
				if (err) {
					log('X3D-init error: ', err);
				}

				if (loopAnimation) {
					self.Animate();
					setInterval(function() {
						keyIndex = 0;
						self.Animate();
					}, 6000);
				}
			}
		);
}

exports.init = function(x3dFilePath) {
	try {
		self.loadFiles();
	} catch (e) {
		log('X3D-init exception cached: ' + e);
	}
}
