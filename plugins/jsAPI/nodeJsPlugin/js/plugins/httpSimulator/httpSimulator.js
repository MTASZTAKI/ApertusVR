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
var moduleManager = require('../../modules/module_manager/module_manager.js');
var request = moduleManager.requireNodeModule('request');
var LineByLineReader = moduleManager.requireNodeModule('line-by-line');
var self = this;

exports.init = function(args) {
	var fileName = moduleManager.pluginPath + args.fileName || moduleManager.pluginPath + 'apertusvr.log';
	var delay = args.delay || 20;
	var lr = new LineByLineReader(fileName);
	var lineNumber = 0;

	var options = {
		uri: moduleManager.httpServer.address + '/api/v1/setproperties',
		method: 'POST',
		json: {}
	};

	lr.on('error', function (err) {
		console.log('line-reader: error: ', err);
	});

	lr.on('line', function (line) {
		lr.pause();

		options.json = JSON.parse(line);
		lineNumber++;
		console.log('line-reader: line> ', lineNumber);

		request.post(options, function (error, response, body) {
				if (error) {
					console.log('Error: ', error);
				}
				// console.log();
				// console.log('request: body: ', body);
				setTimeout(function(){
					lr.resume();
				}, delay);
			}
		);
	});

	lr.on('end', function () {
		console.log('line-reader: all lines are read.');
		self.done(args);
	});
};

exports.done = function(args) {
	console.log('plugin finished');
	if (args) {
		self.init(args);
	}
}
