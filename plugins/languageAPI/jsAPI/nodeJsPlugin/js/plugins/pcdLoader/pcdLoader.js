/*MIT License

Copyright (c) 2016-2019 MTA SZTAKI

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
var logger = require("../../modules/log_manager/log_manager.js");
var fs = require('fs');
var async = moduleManager.requireNodeModule('async');
const path = require('path');
var request = moduleManager.requireNodeModule('request');
var BinaryFile = moduleManager.requireNodeModule('binary-file');
var self = this;

function log(...args) {
	logger.debug(args);
}

function decompressLZF (inData, outLength) {
	var inLength = inData.length
	var outData = new Uint8Array(outLength)
	var inPtr = 0
	var outPtr = 0
	var ctrl
	var len
	var ref
	do {
		ctrl = inData[inPtr++]
		if (ctrl < (1 << 5)) {
			ctrl++
			if (outPtr + ctrl > outLength) throw new Error('Output buffer is not large enough')
			if (inPtr + ctrl > inLength) throw new Error('Invalid compressed data')
			do {
				outData[outPtr++] = inData[inPtr++]
			} while (--ctrl)
		} else {
			len = ctrl >> 5
			ref = outPtr - ((ctrl & 0x1f) << 8) - 1
			if (inPtr >= inLength) throw new Error('Invalid compressed data')
			if (len === 7) {
				len += inData[inPtr++]
				if (inPtr >= inLength) throw new Error('Invalid compressed data')
			}
			ref -= inData[inPtr++]
			if (outPtr + len + 2 > outLength) throw new Error('Output buffer is not large enough')
			if (ref < 0) throw new Error('Invalid compressed data')
			if (ref >= outPtr) throw new Error('Invalid compressed data')
			do {
				outData[outPtr++] = outData[ref++]
			} while (--len + 2)
		}
	} while (inPtr < inLength)
	return outData
}


function parseHeader(binaryData) {
	console.log('PcdLoaderPlugin parseHeader');
	var headerText = ''
	var charArray = new Uint8Array(binaryData)
	var i = 0
	var max = charArray.length
	while (i < max && headerText.search(/[\r\n]DATA\s(\S*)\s/i) === -1) {
		headerText += String.fromCharCode(charArray[i++])
	}
	var result1 = headerText.search(/[\r\n]DATA\s(\S*)\s/i)
	var result2 = /[\r\n]DATA\s(\S*)\s/i.exec(headerText.substr(result1 - 1))

	var header = {}
	header.data = result2[1]
	header.headerLen = result2[0].length + result1
	header.str = headerText.substr(0, header.headerLen)

	// Remove comments
	header.str = header.str.replace(/\#.*/gi, '')
	header.version = /VERSION (.*)/i.exec(header.str)
	if (header.version !== null) {
		header.version = parseFloat(header.version[1])
	}
	header.fields = /FIELDS (.*)/i.exec(header.str)
	if (header.fields !== null) {
		header.fields = header.fields[1].split(' ')
	}
	header.size = /SIZE (.*)/i.exec(header.str)
	if (header.size !== null) {
		header.size = header.size[1].split(' ').map(function (x) {
			return parseInt(x, 10)
		})
	}
	header.type = /TYPE (.*)/i.exec(header.str)
	if (header.type !== null) {
		header.type = header.type[1].split(' ')
	}
	header.count = /COUNT (.*)/i.exec(header.str)
	if (header.count !== null) {
		header.count = header.count[1].split(' ').map(function (x) {
			return parseInt(x, 10)
		})
	}
	header.width = /WIDTH (.*)/i.exec(header.str)
	if (header.width !== null) {
		header.width = parseInt(header.width[1])
	}
	header.height = /HEIGHT (.*)/i.exec(header.str)
	if (header.height !== null) {
		header.height = parseInt(header.height[1])
	}
	header.viewpoint = /VIEWPOINT (.*)/i.exec(header.str)
	if (header.viewpoint !== null) {
		header.viewpoint = header.viewpoint[1]
	}
	header.points = /POINTS (.*)/i.exec(header.str)
	if (header.points !== null) {
		header.points = parseInt(header.points[1], 10)
	}
	if (header.points === null) {
		header.points = header.width * header.height
	}
	if (header.count === null) {
		header.count = []
		for (i = 0; i < header.fields; i++) {
			header.count.push(1)
		}
	}
	header.offset = {}
	var sizeSum = 0
	for (var j = 0; j < header.fields.length; j++) {
		if (header.data === 'ascii') {
			header.offset[header.fields[j]] = j
		} else if (header.data === 'binary') {
			header.offset[header.fields[j]] = sizeSum
			sizeSum += header.size[j]
		} else if (header.data === 'binary_compressed') {
			header.offset[header.fields[j]] = sizeSum
			sizeSum += header.size[j] * header.points
		}
	}
	// For binary only
	header.rowSize = sizeSum
	return header
}

function parseData(size, binaryData, header) {
	console.log('PcdLoaderPlugin parseData, header:', header);
	var offset = header.offset

	var position = false
	if (offset.x !== undefined && offset.y !== undefined && offset.z !== undefined) {
		position = new Float32Array(header.points * 3)
	}

	var color = false
	var color_offset
	if (offset.rgb !== undefined || offset.rgba !== undefined) {
		color = new Float32Array(header.points * 3)
		color_offset = offset.rgb === undefined ? offset.rgba : offset.rgb
	}

	if (header.data === 'ascii') {
		var charArrayView = new Uint8Array(binaryData)
		var dataString = ''
		for (var j = header.headerLen; j < binaryData.byteLength; j++) {
			dataString += String.fromCharCode(charArrayView[j])
		}

		var lines = dataString.split('\n')
		var i3 = 0
		for (var i = 0; i < lines.length; i++, i3 += 3) {
			var line = lines[i].split(' ')
			if (position !== false) {
				position[i3 + 0] = parseFloat(line[offset.x])
				position[i3 + 1] = parseFloat(line[offset.y])
				position[i3 + 2] = parseFloat(line[offset.z])
			}
			if (color !== false) {
				var c
				if (offset.rgba !== undefined) {
					c = new Uint32Array([parseInt(line[offset.rgba])])
				} else if (offset.rgb !== undefined) {
					c = new Float32Array([parseFloat(line[offset.rgb])])
				}
				var dataview = new Uint8Array(c.buffer, 0)
				color[i3 + 2] = dataview[0] / 255.0
				color[i3 + 1] = dataview[1] / 255.0
				color[i3 + 0] = dataview[2] / 255.0
			}
		}
	} else if (header.data === 'binary') {
		var row = 0
		var arrayBuffer = new Uint8Array(binaryData).buffer;
		var dataArrayView = new DataView(arrayBuffer, header.headerLen)
		for (var p = 0; p < header.points; row += header.rowSize, p++) {
			if (position !== false) {
				position[p * 3 + 0] = dataArrayView.getFloat32(row + offset.x, true)
				position[p * 3 + 1] = dataArrayView.getFloat32(row + offset.y, true)
				position[p * 3 + 2] = dataArrayView.getFloat32(row + offset.z, true)
			}
			if (color !== false) {
				color[p * 3 + 2] = dataArrayView.getUint8(row + color_offset + 0) / 255.0
				color[p * 3 + 1] = dataArrayView.getUint8(row + color_offset + 1) / 255.0
				color[p * 3 + 0] = dataArrayView.getUint8(row + color_offset + 2) / 255.0
			}
		}
	} else if (header.data === 'binary_compressed') {
		var sizes = new Uint32Array(binaryData.slice(header.headerLen, header.headerLen + 8))
		var compressedSize = sizes[0]
		var decompressedSize = sizes[1]
		var decompressed = decompressLZF(new Uint8Array(binaryData, header.headerLen + 8, compressedSize), decompressedSize)
		dataArrayView = new DataView(decompressed.buffer)
		for (p = 0; p < header.points; p++) {
			if (position !== false) {
				position[p * 3 + 0] = dataArrayView.getFloat32(offset.x + p * 4, true)
				position[p * 3 + 1] = dataArrayView.getFloat32(offset.y + p * 4, true)
				position[p * 3 + 2] = dataArrayView.getFloat32(offset.z + p * 4, true)
			}
			if (color !== false) {
				color[p * 3 + 2] = dataArrayView.getUint8(color_offset + p * 4 + 0) / 255.0
				color[p * 3 + 1] = dataArrayView.getUint8(color_offset + p * 4 + 1) / 255.0
				color[p * 3 + 0] = dataArrayView.getUint8(color_offset + p * 4 + 2) / 255.0
			}
		}
	}
	return {
		position: Array.prototype.slice.call(position), 
		color: Array.prototype.slice.call(color)
	};
}

function createApertusPointCloud(pointCloud) {
	console.log('PcdLoaderPlugin createApertusPointCloud begin...');
	console.log(ape.bindManager());
	var apeNode = ape.bindManager().createNode(asset.file);
	apeNode.setScale(new ape.nbind.Vector3(asset.scale[0], asset.scale[1], asset.scale[2]));
	apeNode.setOrientation(new ape.nbind.Quaternion(asset.orientation[0], asset.orientation[1], asset.orientation[2], asset.orientation[3]));
	apeNode.setPosition(new ape.nbind.Vector3(asset.position[0], asset.position[1], asset.position[2]));
	var apePointCloud = ape.bindManager().createPointCloud(asset.file);
	apePointCloud.setParameters(pointCloud.position, pointCloud.color, 100000, 1.0, true, 500.0, 500.0, 3.0);
	if (apeNode) {
		apePointCloud.setParentNodeJsPtr(apeNode);
		console.log('PcdLoaderPlugin - this: ' + apePointCloud.getName() + ' - parentNode: ' + apeNode.getName());
	} else
		console.log('PcdLoaderPlugin no parent, thus cannot attach to the pointCloud: ' + apePointCloud.getName());
	console.log('PcdLoaderPlugin createApertusPointCloud end');
}

exports.parsePclAsync = function (callback) {
	const myBinaryFile = new BinaryFile(asset.file, 'r', true);
	(async function () {
		try {
			await myBinaryFile.open();
			var size = await myBinaryFile.size();
			console.log('PcdLoaderPlugin File opened, size: ', size);
			var binaryData = await myBinaryFile.read(size);
			var header = parseHeader(binaryData);
			var pointCloud = parseData(size, binaryData, header);
			createApertusPointCloud(pointCloud);
			await myBinaryFile.close();
			console.log('PcdLoaderPlugin File closed');
		} catch (err) {
			console.log(`There was an error: ${err}`);
		}
	})();
}

var asset;

exports.loadFiles = function() {
	console.log("PcdLoaderPlugin.loadFiles()");
	var configFolderPath = ape.bindManager().getFolderPath();
	console.log('PcdLoaderPlugin configFolderPath: ' + configFolderPath);
	config = require(configFolderPath + '\\ApePcdLoaderPlugin.json');
	asyncFunctions = new Array();
	for (var i = 0; i < config.assets.length; i++) {
		var fn = function (callback) {
			asset = config.assets.pop();
			console.log('PcdLoaderPlugin asset to load: ', asset.file);
			asset.file = moduleManager.sourcePath + asset.file;
			self.parsePclAsync(function() {
				console.log('PcdLoaderPlugin Pcl-parsing done: ' + filePath + ' scale: ' + asset.scale + ' position: ' + asset.position + ' orientation: ' + asset.orientation);
				callback(null);
			});
		}
		asyncFunctions.push(fn);
	}
	async.waterfall(
			asyncFunctions,
			function(err, result) {
				console.log("PcdLoaderPlugin async tasks done");
				if (err) {
					console.log('PcdLoaderPlugin Pcd-init error: ', err);
				}
			}
		);
}

exports.init = function(PclFilePath) {
	try {
		self.loadFiles();
	} catch (e) {
		console.log('PcdLoaderPlugin Pcd-init exception cached: ' + e);
	}
}
