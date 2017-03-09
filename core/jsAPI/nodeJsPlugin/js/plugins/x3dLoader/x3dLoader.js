var utils = require('apertusvr/js/utils.js');
var ape = require('apertusvr/js/ape.js');
var cheerio = require('cheerio');
var fs = require('fs');
const uuidV1 = require('uuid/v1');
var self = this;

// IndexedFaceSet, IndexedLineSet

var nameCounter = 0;

exports.parseCoordIndexAttr = function(currentItem, callback)
{
  if (!utils.isDefined(currentItem)) {
    callback('currentItem is not defined!', null);
    return false;
  }

  var coordinates = new Array();
  var erdosCoordinates = new Array();
  var coordIndex = currentItem.attr('coordIndex');

  if (!utils.isDefined(coordIndex)) {
    callback('coordIndex is not defined!', null);
    return false;
  }

  var itemsArr = coordIndex.replace('\n', ' ').replace(/ +(?= )/g,'').trim().split(' ');
  if (itemsArr.length < 4) {
    callback('Array length is less than 4!', null);
    return false;
  }

  for (var i = 0; i < itemsArr.length; i++) {
    coordinates.push(itemsArr[i]);
  }

  var tempArr = new Array();
  for (var i = 0; i < coordinates.length; i++) {
    if (coordinates[i] == -1)
    {
      // erdos-specific mirroring
      if (i % 5 == 0) {
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
    }
    else {
      tempArr.push(coordinates[i]);
    }
  }

  callback(null, erdosCoordinates);
  return true;
}

exports.parseColorIndexAttr = function(currentItem)
{
  var colorIndices = new Array();
  var colorIndex = currentItem.attr('colorIndex');
  if (utils.isDefined(colorIndex)) {
    var itemsArr = colorIndex.trim().split(' ');
    for (var i = 0; i < itemsArr.length; i++) {
      colorIndices.push(Number(itemsArr[i]));
    }
  }
  return colorIndices;
}

exports.parseCoordinatePointAttr = function(currentItem, callback)
{
  if (!utils.isDefined(currentItem)) {
    callback('currentItem is not defined!', null);
    return false;
  }

  var pointsArr = new Array();
  var coordinate = currentItem.find('Coordinate').first();

  if (!utils.isDefined(coordinate)) {
    callback('coordinate is not defined!', null);
    return false;
  }

  var pointAttr = coordinate.attr('point');
  if (!utils.isDefined(pointAttr)) {
    callback('pointAttr is not defined!', null);
    return false;
  }

  var itemsArr = pointAttr.replace('\n', ' ').replace(/ +(?= )/g,'').trim().split(' ');
  if (itemsArr.length == 0) {
    callback('Array length is 0!', null);
    return false;
  }

  for (var j = 0; j < itemsArr.length; j++) {
    pointsArr.push(Number(itemsArr[j]));
  }

  callback(null, pointsArr);
  return true;
}

// Transform

exports.parseTranslationAttr = function(currentItem)
{
  var translation = currentItem.attr('translation');
  if (utils.isDefined(translation)) {
    console.log(' - translation: ' + translation);
    var itemArr = translation.trim().split(' ');
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
    var itemArr = rotation.trim().split(' ');
    if (itemArr.length == 4) {
      return new ape.nbind.Quaternion(Number(itemArr[0]), Number(itemArr[1]), Number(itemArr[2]), Number(itemArr[3]));
    }
  }
  return new ape.nbind.Quaternion();
}

exports.parseScaleAttr = function(currentItem)
{
  var scale = currentItem.attr('scale');
  if (utils.isDefined(scale)) {
    console.log(' - scale: ' + scale);
    var itemArr = scale.trim().split(' ');
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
    return 1.0;
}

exports.parseDiffuseColorAttr = function (currentItem, transparency) {
    var diffuseColor = currentItem.attr('diffuseColor');
    if (utils.isDefined(diffuseColor)) {
        console.log(' - diffuseColor: ' + diffuseColor);
        var itemArr = diffuseColor.trim().split(' ');
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
        var itemArr = specularColor.trim().split(' ');
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
    console.log(diffuseColor.toString());
    console.log(specularColor.toString());
    pbsPass.setDiffuseColor(diffuseColor);
    pbsPass.setSpecularColor(specularColor);
    manualMaterial.setPbsPass(pbsPass);
    parentGeometry.setManualMaterial(manualMaterial);
}

// ---

exports.parseItem = function(parentItem, currentItem, parentNodeObj)
{
  var typeName = currentItem[0].type;
  var tagName = currentItem[0].tagName || currentItem[0].name;
  var itemName = (currentItem[0].itemName || currentItem.attr('DEF') || 'item' + nameCounter);
  nameCounter++;
  currentItem[0].itemName = itemName;
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
    else if (tagName == 'indexedfaceset') {
      var indexedFaceSetObj = ape.nbind.JsBindManager().createIndexedFaceSet(currentItem[0].itemName);
      console.log('indexedFaceSet is created with name: ' + currentItem[0].itemName);
      console.log(indexedFaceSetObj.getParameters().toString());

      var coordinatePointsArr;
      var parseCoordinatePointAttrRes = self.parseCoordinatePointAttr(currentItem, function(error, pointsArr){
        if (error) {
          console.log('Error: ' + error);
          return;
        }
        coordinatePointsArr = pointsArr;
        console.log(' - coordinate.point: ' + coordinatePointsArr);
      });
      if (!parseCoordinatePointAttrRes) {
        console.log('Error in parseCoordinatePointAttr(), returning.');
        return;
      }

      var coordIndexArr;
      var parseCoordIndexAttrRes = self.parseCoordIndexAttr(currentItem, function(error, indexArr) {
        if (error) {
          console.log('Error: ' + error);
          return;
        }
        coordIndexArr = indexArr;
        console.log(' - coordIndex: ' + coordIndexArr);
      });
      if (!parseCoordIndexAttrRes) {
        console.log('Error in parseCoordIndexAttr(), returning.');
        return;
      }

      var solid = currentItem.attr('solid');
      if (utils.isDefined(solid)) {
        console.log(' - solid: ' + solid);
      }

      indexedFaceSetObj.setParameters(coordinatePointsArr, coordIndexArr);
      console.log(indexedFaceSetObj.getParameters().toString());

      console.log('parentNodeObj: ' + parentNodeObj);
      if (parentNodeObj) {
        console.log('parentNodeObj is not NULL, setting parentNode to: ' + parentNodeObj.getName());
        indexedFaceSetObj.setParentNodeJsPtr(parentNodeObj);
      }

      var matItem = currentItem.siblings('Appearance').first().children('Material').first();
      self.parseMaterial(matItem, indexedFaceSetObj);
    }
    else if (tagName == 'indexedlineset') {
      // var coordIndexArr = self.parseCoordIndexAttr(currentItem);
      // console.log(' - coordIndex: ' + coordIndexArr);
      //
      // var colorIndexArr = self.parseCoordIndexAttr(currentItem);
      // console.log(' - colorIndex: ' + colorIndexArr);
      //
      // var colorPerVertex = currentItem.attr('colorPerVertex');
      // if (utils.isDefined(colorPerVertex)) {
      //   console.log(' - colorPerVertex: ' + colorPerVertex);
      // }
      //
      // var coordinatePointsArr = self.parseCoordinatePointAttr(currentItem);
      // console.log(' - coordinate.point: ' + coordinatePointsArr);
    }
    else if (tagName == 'transform') {
      var nodeObj = ape.nbind.JsBindManager().createNode(currentItem[0].itemName);
      console.log('node created with name: ' + nodeObj.getName());
      nodeLevel++;

      var position = self.parseTranslationAttr(currentItem);
      nodeObj.setPosition(position);
      console.log(nodeObj.getPosition().toString());

      var orientation = self.parseRotationAttr(currentItem);
      nodeObj.setOrientation(orientation);
      console.log(nodeObj.getOrientation().toString());

      var scale = self.parseScaleAttr(currentItem);
      nodeObj.setScale(scale);
      console.log(nodeObj.getScale().toString());

      console.log('parentNodeObj: ' + parentNodeObj);
      if (parentNodeObj) {
        console.log('parentNodeObj is not NULL, setting parentNode to: ' + parentNodeObj.getName());
        nodeObj.setParentNodeJsPtr(parentNodeObj);
      }

      return nodeObj;
    }
    else if (tagName == 'group') {
      var nodeObj = ape.nbind.JsBindManager().createNode(currentItem[0].itemName);
      console.log('group node created with name: ' + nodeObj.getName());
      nodeLevel++;

      console.log('parentNodeObj: ' + parentNodeObj);
      if (parentNodeObj) {
        console.log('parentNodeObj is not NULL, setting parentNode to: ' + parentNodeObj.getName());
        nodeObj.setParentNodeJsPtr(parentNodeObj);
      }

      return nodeObj;
    }
    else {

    }
  }
}


var nodeLevelMap = {};
var nodeLevel = 0;
var nodeLevelTmp = 0;

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

  console.log('currentNode: ' + currentNode);
  console.log('nodeLevel: ' + nodeLevel);

  nodeLevelTmp = nodeLevel;

  $(childItem).children().each(function(i, elem) {
    var currentChild = $(this);
    self.parseTree($, $(currentChild).parent(), $(currentChild), nodeLevelMap[nodeLevel]);
  });

  if (currentNode) {
    nodeLevel = nodeLevel - 1;
  }

  return;
};

exports.parseX3D = function(x3dFilePath) {
  var $ = cheerio.load(fs.readFileSync(x3dFilePath), {
    xmlMode: true,
    lowerCaseTags: true
  });
  self.parseTree($, null, $('X3D'), null);
};
