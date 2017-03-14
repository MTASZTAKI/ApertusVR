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

var utils = require('apertusvr/js/utils.js');
var ape = require('apertusvr/js/ape.js');
var cheerio = require('cheerio');
var fs = require('fs');
const uuidV1 = require('uuid/v1');
const path = require('path');
var self = this;

// IndexedFaceSet, IndexedLineSet

var nameCounter = 0;

function splitX3DAttr(str) {
    var res = str;
    res = res.replace(/[\r\n]+/g, '');
    res = res.replace(/ +(?= )/g, '');
    res = res.trim();
    res = res.split(' ');
    console.log('--------------------------------------------------------------------------------');
    console.log('res: ' + res);
    return res;
    //return str.replace(/[\r\n]+/g, '').replace(/ +(?= )/g,'').trim().split(' ');
}

exports.parseCoordIndexAttr = function(currentItem)
{
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
  for (var i = 0; i < coordinates.length; i++)
  {
    if (coordinates[i] == -1)
    {
        // erdos-specific mirroring
        if (indexCount == 5)
        {
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
    }
    else
    {
        tempArr.push(coordinates[i]);
    }
    indexCount++;
  }

  console.log(' - coordIndex: ' + erdosCoordinates);
  return erdosCoordinates;
}

exports.parseColorIndexAttr = function(currentItem)
{
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

exports.parseCoordinatePointAttr = function(currentItem)
{
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

  console.log(' - coordinatePoints: ' + pointsArr);
  return pointsArr;
}

exports.parsePositionInterpolatorKeyValuesAttr = function (currentItem) {
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

    console.log(' - keyValue: ' + items);
    return items;
}

exports.parseOrientationInterpolatorKeyValuesAttr = function (currentItem) {
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

    console.log(' - keyValue: ' + items);
    return items;
}

// Transform

exports.parseTranslationAttr = function(currentItem)
{
  var translation = currentItem.attr('translation');
  if (utils.isDefined(translation)) {
    console.log(' - translation: ' + translation);
    var itemArr = splitX3DAttr(translation);
      if (itemArr.length == 3) {
        return new ape.nbind.Vector3(Number(itemArr[0]), Number(itemArr[1]), Number(itemArr[2]));
      }
  }
  return new ape.nbind.Vector3();
}

exports.parseRotationAttr = function(currentItem)
{
  var rotation = currentItem.attr('rotation');
  if (utils.isDefined(rotation)) {
    console.log(' - rotation: ' + rotation);
    var itemArr = splitX3DAttr(rotation);
    if (itemArr.length == 4) {
      var axis = ape.nbind.Vector3(Number(itemArr[0]), Number(itemArr[1]), Number(itemArr[2]));
      var radian = ape.nbind.Radian(Number(itemArr[3]));
      return new ape.nbind.Quaternion(radian, axis);
    }
  }
  return new ape.nbind.Quaternion();
}

exports.parseScaleAttr = function(currentItem)
{
  var scale = currentItem.attr('scale');
  if (utils.isDefined(scale)) {
    console.log(' - scale: ' + scale);
    var itemArr = splitX3DAttr(scale);
    if (itemArr.length == 3) {
      return new ape.nbind.Vector3(Number(itemArr[0]), Number(itemArr[1]), Number(itemArr[2]));
    }
  }
  return new ape.nbind.Vector3(1, 1, 1);
}

exports.parseTransparencyAttr = function (currentItem) {
    var transparency = currentItem.attr('transparency');
    if (utils.isDefined(transparency)) {
        console.log(' - transparency: ' + transparency);
        return Number(transparency);
    }
    return 0.0;
}

exports.parseDiffuseColorAttr = function (currentItem, transparency) {
    var diffuseColor = currentItem.attr('diffuseColor');
    if (utils.isDefined(diffuseColor)) {
        console.log(' - diffuseColor: ' + diffuseColor);
        var itemArr = splitX3DAttr(diffuseColor);
        if (itemArr.length == 3) {
            return new ape.nbind.Color(Number(itemArr[0]), Number(itemArr[1]), Number(itemArr[2]), 1 - transparency);
        }
    }
    return new ape.nbind.Color(1, 1, 1, 1);
}

exports.parseSpecularColorAttr = function (currentItem, transparency) {
    var specularColor = currentItem.attr('specularColor');
    if (utils.isDefined(specularColor)) {
        console.log(' - specularColor: ' + specularColor);
        var itemArr = splitX3DAttr(specularColor);
        if (itemArr.length == 3) {
            return new ape.nbind.Color(Number(itemArr[0]), Number(itemArr[1]), Number(itemArr[2]), 1 - transparency);
        }
    }
    return new ape.nbind.Color(1, 1, 1, 1);
}

exports.parseMaterial = function (currentItem, parentGeometry) {
    var manualMaterial = ape.nbind.JsBindManager().createManualMaterial(currentItem[0].itemName);
    var pbsPass = ape.nbind.JsBindManager().createPbsPass(currentItem[0].itemName + 'PbsPass');
    var transparency = self.parseTransparencyAttr(currentItem);
    var diffuseColor = self.parseDiffuseColorAttr(currentItem, transparency);
    var specularColor = self.parseSpecularColorAttr(currentItem, transparency);
    pbsPass.setDiffuseColor(diffuseColor);
    pbsPass.setSpecularColor(specularColor);
    manualMaterial.setPbsPass(pbsPass);
    return manualMaterial;
    //parentGeometry.setManualMaterial(manualMaterial);
}

exports.parseColorAttr = function (currentItem) {
    var color = currentItem.find('color').first();
    if (!utils.isDefined(color)) {
        throw 'color attrib is not defined!';
        return null;
    }
    var colorAttr = color.attr('color');
    if (utils.isDefined(colorAttr)) {
        console.log(' - color: ' + colorAttr);
        var itemArr = splitX3DAttr(colorAttr);
        if (itemArr.length == 3) {
            return new ape.nbind.Color(Number(itemArr[0]), Number(itemArr[1]), Number(itemArr[2]), 1);
        }
    }
    return new ape.nbind.Color(1, 1, 1, 1);
}

exports.parseDimensionsAttr = function (currentItem) {
    var dimensions = currentItem.attr('size');
    if (utils.isDefined(dimensions)) {
        console.log(' - dimensions: ' + dimensions);
        var itemArr = splitX3DAttr(dimensions);
        if (itemArr.length == 3) {
            return new ape.nbind.Vector3(Number(itemArr[0]), Number(itemArr[1]), Number(itemArr[2]));
        }
    }
    return new ape.nbind.Vector3(1, 1, 1);
}

// ---

exports.parseItem = function(parentItem, currentItem, parentNodeObj)
{
  var typeName = currentItem[0].type;
  var tagName = currentItem[0].tagName || currentItem[0].name;
  var itemName = (currentItem[0].itemName || currentItem.attr('DEF') || 'item' + nameCounter);
  nameCounter++;
  currentItem[0].itemName = itemName + currentlyParsedFileName;
  console.log(itemName + ' - ' + typeName + ' - ' + tagName);

  if (!typeName || !tagName) {
    console.log('currentItem has no typeName or tagName, returning.');
    return true; // continue;
  }

  if (typeName == 'tag') {
    if (tagName == '?xml') {
      //
    }
    else if (tagName == 'x3d') {
      //
    }
    else if (tagName == 'meta') {
      var name = currentItem.attr('name');
      if (utils.isDefined(name)) {
        console.log(' - name: ' + name);
        //  TODO: create a textGeometry
      }

      var content = currentItem.attr('content');
      if (utils.isDefined(content)) {
        console.log(' - content: ' + content);
        //  TODO: create a textGeometry
      }
    }
    else if (tagName == 'scene') {
      //
    }
    else if (tagName == 'worldinfo') {
      var info = currentItem.attr('info');
      if (utils.isDefined(info)) {
        console.log(' - info: ' + info);
        //  TODO: create a textGeometry
      }
    }
    else if (tagName == 'viewpoint') {
      //
    }
    else if (tagName == 'shape') {
        var use = currentItem.attr('USE');
        if (utils.isDefined(use)) {
            var geometryName = use + currentlyParsedFileName;
            var fileGeometryObj = ape.nbind.JsBindManager().createFileGeometry(currentItem[0].itemName);
            fileGeometryObj.setFileName(geometryName);
            console.log('USE: ' + fileGeometryObj.getName());

            if (parentNodeObj) {
                fileGeometryObj.setParentNodeJsPtr(parentNodeObj);
                console.log(' - this: ' + fileGeometryObj.getName() + ' - parentNode: ' + parentNodeObj.getName());
            }
        }
    }
    else if (tagName == 'indexedfaceset') {
        var groupNodeObjName = groupNodeObj.getName();
        console.log('- indexedfaceset:' + groupNodeObjName);
        var HANDLING = groupNodeObjName.indexOf("handling");
        console.log('- HANDLING:' + HANDLING);
        if (HANDLING < 0) {
            var indexedFaceSetObj = ape.nbind.JsBindManager().createIndexedFaceSet(currentItem[0].itemName);
            var coordinatePointsArr = self.parseCoordinatePointAttr(currentItem);
            var coordIndexArr = self.parseCoordIndexAttr(currentItem);
            var matItem = currentItem.siblings('Appearance').first().children('Material').first();
            var materialObj = self.parseMaterial(matItem, indexedFaceSetObj);
            indexedFaceSetObj.setParameters(groupNodeObjName, coordinatePointsArr, coordIndexArr, materialObj);

            if (lastGroupNodeObjName != groupNodeObjName) {
                if (groupNodeObj) {
                    indexedFaceSetObj.setParentNodeJsPtr(groupNodeObj);
                    console.log('- groupNodeObj:' + groupNodeObjName);
                }
                lastGroupNodeObjName = groupNodeObjName;
            }
        }
    }
    else if (tagName == 'box') {
        var boxSetObj = ape.nbind.JsBindManager().createBox(currentItem[0].itemName);
        var dimensionsAtrr = self.parseDimensionsAttr(currentItem);
        boxSetObj.setParameters(dimensionsAtrr);
        var matItem = currentItem.siblings('Appearance').first().children('Material').first();
        self.parseMaterial(matItem, boxSetObj);

        if (parentNodeObj) {
            boxSetObj.setParentNodeJsPtr(parentNodeObj);
            console.log(' - this: ' + boxSetObj.getName() + ' - parentNode: ' + parentNodeObj.getName());
        }
    }
    else if (tagName == 'indexedlineset') {
        var indexedLineSetObj = ape.nbind.JsBindManager().createIndexedLineSet(currentItem[0].itemName);
        var coordinatePointsArr = self.parseCoordinatePointAttr(currentItem);
        var coordIndexArr = self.parseCoordIndexAttr(currentItem);
        var color = self.parseColorAttr(currentItem);
        indexedLineSetObj.setParameters(coordinatePointsArr, coordIndexArr, color);

        if (parentNodeObj) {
            indexedLineSetObj.setParentNodeJsPtr(parentNodeObj);
            console.log(' - this: ' + indexedLineSetObj.getName() + ' - parentNode: ' + parentNodeObj.getName());
        }
    }
    else if (tagName == 'transform') {
        var nodeObj = ape.nbind.JsBindManager().createNode(currentItem[0].itemName);
        nodeLevel++;

        var position = self.parseTranslationAttr(currentItem);
        nodeObj.setPosition(position);

        if (nodeObj.getName() == 'WORLD' + currentlyParsedFileName) {
            console.log(' - WorldTransform: ');
            nodeObj.setScale(new ape.nbind.Vector3(0.1, 0.1, 0.1));
            nodeObj.setOrientation(new ape.nbind.Quaternion(0.7071, -0.7071, 0, 0));
            if (currentlyParsedFileName == 'weldingFixture') {
                nodeObj.setPosition(new ape.nbind.Vector3(20000, 0, 0));
            }
        }
        else {
            var orientation = self.parseRotationAttr(currentItem);
            nodeObj.setOrientation(orientation);
            var scale = self.parseScaleAttr(currentItem);
            nodeObj.setScale(scale);
        }

        if (parentNodeObj) {
            nodeObj.setParentNodeJsPtr(parentNodeObj);
            console.log(' - this: ' + nodeObj.getName() + ' - parentNode: ' + parentNodeObj.getName());
        }

        return nodeObj;
    }
    else if (tagName == 'group') {
        var nodeObj = ape.nbind.JsBindManager().createNode(currentItem[0].itemName);
        nodeLevel++;

        if (parentNodeObj) {
            nodeObj.setParentNodeJsPtr(parentNodeObj);
            console.log(' - this: ' + nodeObj.getName() + ' - parentNode: ' + parentNodeObj.getName());
        }
        groupNodeObj = nodeObj;
        console.log('groupChanged: ' + groupNodeObj.getName());
        return nodeObj;
    }
    else if (tagName == 'switch') {
        var nodeObj = ape.nbind.JsBindManager().createNode(currentItem[0].itemName);
        nodeLevel++;

        if (parentNodeObj) {
            nodeObj.setParentNodeJsPtr(parentNodeObj);
            console.log(' - this: ' + nodeObj.getName() + ' - parentNode: ' + parentNodeObj.getName());
        }
        return nodeObj;
    }
    else if (tagName == 'orientationinterpolator') {
        var orientationInterpolator = { type: 'orientation', name: currentItem[0].itemName, keys: new Array(), keyValues: new Array(), nodeName: '', nodeObj : 0 };
        var keys = splitX3DAttr(currentItem.attr('key'));
        var keyValues = self.parseOrientationInterpolatorKeyValuesAttr(currentItem);
        orientationInterpolator.keys = keys;
        orientationInterpolator.keyValues = keyValues;
        interpolatorArr.push(orientationInterpolator);
        console.log('OrientationInterpolator: ' + currentItem[0].itemName);
    }
    else if (tagName == 'positioninterpolator') {
        var positionInterpolator = { type: 'position', name: currentItem[0].itemName, keys: new Array(), keyValues: new Array(), nodeName: '', nodeObj : 0 };
        var keys = splitX3DAttr(currentItem.attr('key'));
        var keyValues = self.parsePositionInterpolatorKeyValuesAttr(currentItem);
        positionInterpolator.keys = keys;
        positionInterpolator.keyValues = keyValues;
        interpolatorArr.push(positionInterpolator);
        console.log('PositionInterpolator: ' + currentItem[0].itemName);
    }
    else if (tagName == 'route') {
        var interpolatorName = currentItem.attr('fromNode') + currentlyParsedFileName;
        var nodeName = currentItem.attr('toNode') + currentlyParsedFileName;
        console.log('ROUTE: ' + interpolatorName);
        for (var i = 0; i < interpolatorArr.length; i++) {
            if (interpolatorArr[i].name == interpolatorName) {
                interpolatorArr[i].nodeName = nodeName;
                ape.nbind.JsBindManager().getNode(interpolatorArr[i].nodeName, function (error, object) {
                    if (error) {
                        console.log(error);
                        return;
                    }
                    interpolatorArr[i].nodeObj = object;
                });
            }
        }
    }
    else if (tagName == 'timesensor') {
        loopAnimation = currentItem.attr('loop');
        cycleIntervalAnimation = Number(currentItem.attr('cycleInterval'));
    }
    else {

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
var currentlyParsedFileName = '';

exports.parseTree = function($, parentItem, childItem, parentNodeObj) {
  if (!childItem) {
    return;
  }

  var currentNode;
    try {

        currentNode = self.parseItem(parentItem, childItem, parentNodeObj);
  } catch (e) {
    console.log('Exception cached: ' + e);
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

exports.parseX3D = function(x3dFilePath) {
  var $ = cheerio.load(fs.readFileSync(x3dFilePath), {
    xmlMode: true,
    lowerCaseTags: true
  });
  self.parseTree($, null, $('X3D'), null);
}

exports.Animate = function () {

    function DoInterpolationSteps() {
        //console.log('keyIndex:' + keyIndex);
        for (var interpolatorID = 0; interpolatorID < interpolatorArr.length; interpolatorID++) {
            if (keyIndex < interpolatorArr[interpolatorID].keyValues.length) {
                if (interpolatorArr[interpolatorID].type == 'position') {
                    interpolatorArr[interpolatorID].nodeObj.setPosition(interpolatorArr[interpolatorID].keyValues[keyIndex]);
                }
                else if (interpolatorArr[interpolatorID].type == 'orientation') {
                    interpolatorArr[interpolatorID].nodeObj.setOrientation(interpolatorArr[interpolatorID].keyValues[keyIndex]);
                }
            }
        }
        keyIndex++;
    }
    console.log('X3D-animation play begin:');

    for (var i = 0; i < 200; i++) {
        setTimeout(function () {
            DoInterpolationSteps();
        }, 25 * i);
        //console.log('timed out for:' + 25 * i);
    }

    console.log('X3D-animation play end:');
}

exports.init = function(x3dFilePath) {
  currentlyParsedFileName = 'weldingFixture';
  self.parseX3D('node_modules/apertusvr/js/plugins/x3dLoader/samples/' + currentlyParsedFileName + '.x3d');
  console.log('X3D-parsing done: ' + currentlyParsedFileName);
  currentlyParsedFileName = 'cell';
  self.parseX3D('node_modules/apertusvr/js/plugins/x3dLoader/samples/' + currentlyParsedFileName + '.x3d');
  console.log('X3D-parsing done: ' + currentlyParsedFileName);
  currentlyParsedFileName = 'cellAnim';
  self.parseX3D('node_modules/apertusvr/js/plugins/x3dLoader/samples/' + currentlyParsedFileName + '.x3d');
  console.log('X3D-parsing done: ' + currentlyParsedFileName);
  self.Animate();
  if (loopAnimation) {
      setInterval(function () {
          keyIndex = 0;
          self.Animate();
      }, 6000);
  }
  console.log("x3dLoader end")
}
