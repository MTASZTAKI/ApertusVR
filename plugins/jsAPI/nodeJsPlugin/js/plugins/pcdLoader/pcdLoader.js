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
const BinaryFile = require('binary-file');
var self = this;

function log(...args) {
	logger.debug(args);
}

function parseHeader( data ) {
		var PCDheader = {};
		var result1 = data.search( /[\r\n]DATA\s(\S*)\s/i );
		var result2 = /[\r\n]DATA\s(\S*)\s/i.exec( data.substr( result1 - 1 ) );
		PCDheader.data = result2[ 1 ];
		PCDheader.headerLen = result2[ 0 ].length + result1;
		PCDheader.str = data.substr( 0, PCDheader.headerLen );
		// remove comments
		PCDheader.str = PCDheader.str.replace( /\#.*/gi, '' );
		// parse
		PCDheader.version = /VERSION (.*)/i.exec( PCDheader.str );
		PCDheader.fields = /FIELDS (.*)/i.exec( PCDheader.str );
		PCDheader.size = /SIZE (.*)/i.exec( PCDheader.str );
		PCDheader.type = /TYPE (.*)/i.exec( PCDheader.str );
		PCDheader.count = /COUNT (.*)/i.exec( PCDheader.str );
		PCDheader.width = /WIDTH (.*)/i.exec( PCDheader.str );
		PCDheader.height = /HEIGHT (.*)/i.exec( PCDheader.str );
		PCDheader.viewpoint = /VIEWPOINT (.*)/i.exec( PCDheader.str );
		PCDheader.points = /POINTS (.*)/i.exec( PCDheader.str );
		// evaluate
		if ( PCDheader.version !== null )
			PCDheader.version = parseFloat( PCDheader.version[ 1 ] );

		if ( PCDheader.fields !== null )
			PCDheader.fields = PCDheader.fields[ 1 ].split( ' ' );

		if ( PCDheader.type !== null )
			PCDheader.type = PCDheader.type[ 1 ].split( ' ' );

		if ( PCDheader.width !== null )
			PCDheader.width = parseInt( PCDheader.width[ 1 ] );

		if ( PCDheader.height !== null )
			PCDheader.height = parseInt( PCDheader.height[ 1 ] );

		if ( PCDheader.viewpoint !== null )
			PCDheader.viewpoint = PCDheader.viewpoint[ 1 ];

		if ( PCDheader.points !== null )
			PCDheader.points = parseInt( PCDheader.points[ 1 ], 10 );

		if ( PCDheader.points === null )
			PCDheader.points = PCDheader.width * PCDheader.height;

		if ( PCDheader.size !== null ) {
			PCDheader.size = PCDheader.size[ 1 ].split( ' ' ).map( function ( x ) {
				return parseInt( x, 10 );
			} );
		}
		if ( PCDheader.count !== null ) {
			PCDheader.count = PCDheader.count[ 1 ].split( ' ' ).map( function ( x ) {
				return parseInt( x, 10 );
			} );
		} else {
			PCDheader.count = [];
			for ( var i = 0, l = PCDheader.fields.length; i < l; i ++ ) {
				PCDheader.count.push( 1 );
			}
		}
		PCDheader.offset = {};
		var sizeSum = 0;
		for ( var i = 0, l = PCDheader.fields.length; i < l; i ++ ) {
			if ( PCDheader.data === 'ascii' ) {
				PCDheader.offset[ PCDheader.fields[ i ] ] = i;
			} else {
				PCDheader.offset[ PCDheader.fields[ i ] ] = sizeSum;
				sizeSum += PCDheader.size[ i ];
			}
		}
		// for binary only
		PCDheader.rowSize = sizeSum;
		return PCDheader;
}

function parseData( PCDheader ) {
	var position = [];
	var normal = [];
	var color = [];
	// ascii
	if ( PCDheader.data === 'ascii' ) {
		var offset = PCDheader.offset;
		var pcdData = textData.substr( PCDheader.headerLen );
		var lines = pcdData.split( '\n' );
		for ( var i = 0, l = lines.length; i < l; i ++ ) {
			if ( lines[ i ] === '' ) continue;
			var line = lines[ i ].split( ' ' );
			if ( offset.x !== undefined ) {
				position.push( parseFloat( line[ offset.x ] ) );
				position.push( parseFloat( line[ offset.y ] ) );
				position.push( parseFloat( line[ offset.z ] ) );
			}
			if ( offset.rgb !== undefined ) {
				var rgb = parseFloat( line[ offset.rgb ] );
				var r = ( rgb >> 16 ) & 0x0000ff;
				var g = ( rgb >> 8 ) & 0x0000ff;
				var b = ( rgb >> 0 ) & 0x0000ff;
				color.push( r / 255, g / 255, b / 255 );
			}
			if ( offset.normal_x !== undefined ) {
				normal.push( parseFloat( line[ offset.normal_x ] ) );
				normal.push( parseFloat( line[ offset.normal_y ] ) );
				normal.push( parseFloat( line[ offset.normal_z ] ) );
			}
		}
	}
	// binary
	if ( PCDheader.data === 'binary_compressed' ) {
		console.error( 'THREE.PCDLoader: binary_compressed files are not supported' );
		return;
	}
	if ( PCDheader.data === 'binary' ) {
		var dataview = new DataView( data, PCDheader.headerLen );
		var offset = PCDheader.offset;
		for ( var i = 0, row = 0; i < PCDheader.points; i ++, row += PCDheader.rowSize ) {
			if ( offset.x !== undefined ) {
				position.push( dataview.getFloat32( row + offset.x, this.littleEndian ) );
				position.push( dataview.getFloat32( row + offset.y, this.littleEndian ) );
				position.push( dataview.getFloat32( row + offset.z, this.littleEndian ) );
			}
			if ( offset.rgb !== undefined ) {
				color.push( dataview.getUint8( row + offset.rgb + 2 ) / 255.0 );
				color.push( dataview.getUint8( row + offset.rgb + 1 ) / 255.0 );
				color.push( dataview.getUint8( row + offset.rgb + 0 ) / 255.0 );
			}
			if ( offset.normal_x !== undefined ) {
				normal.push( dataview.getFloat32( row + offset.normal_x, this.littleEndian ) );
				normal.push( dataview.getFloat32( row + offset.normal_y, this.littleEndian ) );
				normal.push( dataview.getFloat32( row + offset.normal_z, this.littleEndian ) );
			}
		}
	}
	 return {
        position: position, 
        color: color,
		normal: normal
    };
}

function createApertusPointCloud( data ) {
	var apeNode = ape.nbind.JsBindManager().createNode(asset.file);
	apeNode.setScale(new ape.nbind.Vector3(asset.scale[0], asset.scale[1], asset.scale[2]));
	apeNode.setOrientation(new ape.nbind.Quaternion(asset.orientation[0], asset.orientation[1], asset.orientation[2], asset.orientation[3]));
	apeNode.setPosition(new ape.nbind.Vector3(asset.position[0], asset.position[1], asset.position[2]));
	var apePointCloud = ape.nbind.JsBindManager().createPointCloud(asset.file);
	apePointCloud.setParameters(data.position, data.color, 100000, 1.0, true, 500.0, 500.0, 3.0);
	if (apeNode) {
		apePointCloud.setParentNodeJsPtr(apeNode);
		log(' - this: ' + apePointCloud.getName() + ' - parentNode: ' + apeNode.getName());
	} else
		log('no parent, thus cannot attach to the pointCloud: ' + apePointCloud.getName());
}

exports.parsePclAsync = function (callback) {
	const myBinaryFile = new BinaryFile(asset.file, 'r', true);
	myBinaryFile.open();
    console.log('File opened');
    var PCDheader = parseHeader(myBinaryFile);
	var data = parseData(PCDheader);
	createApertusPointCloud(asset.file, data);
    console.log(`File read: ${string}`);
}

var asset;

exports.loadFiles = function() {
	log("PcdLoaderPlugin.loadFiles()");
	var configFolderPath = ape.nbind.JsBindManager().getFolderPath();
	log('PcdLoaderPlugin configFolderPath: ' + configFolderPath);
	config = require(configFolderPath + '\\ApePcdLoaderPlugin.json');
	asyncFunctions = new Array();
	for (var i = 0; i < config.assets.length; i++) {
		var fn = function (callback) {
			asset = config.assets.pop();
			log('PcdLoaderPlugin asset to load: ', asset.file);
			var filePath = moduleManager.sourcePath + asset.file;
			var scale = asset.scale;
			var position = asset.position;
			var orientation = asset.orientation;
			self.parsePclAsync(function() {
				log('Pcl-parsing done: ' + filePath + ' scale: ' + scale + ' position: ' + position + ' orientation: ' + orientation);
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
					log('Pcl-init error: ', err);
				}
			}
		);
}

exports.init = function(PclFilePath) {
	try {
		self.loadFiles();
	} catch (e) {
		log('Pcl-init exception cached: ' + e);
	}
}
