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

var ape = require('../ape.js');
var async = ape.requireNodeModule('async');
exports.moduleTag = 'ApeJsPluginLoader';

// extend ape module
exports.loadPlugins = function() {
	console.log(this.moduleTag, "test from ApeJsPluginLoader");

	var configFolderPath = ape.nbind.JsBindManager().getFolderPath();
	console.log('js configFolderPath: ' + configFolderPath);

	var config = require(configFolderPath + '\\ApeNodeJsPlugin.json');
	console.log('js plugins to start: ', config.jsPlugins);

	var q = async.queue(function(task, callback) {
		console.log(task.name + ' init function called');
		callback();
	}, config.jsPlugins.length);

	q.drain = function() {
		console.log('all js plugins have been started');
	};

	for (var i = 0; i < config.jsPlugins.length; i++) {
		var pluginFilePath = config.jsPlugins[i];
		var plugin = require(ape.sourcePath + pluginFilePath);
		q.push({
			name: pluginFilePath
		}, function(err) {
			plugin.init();
		});
	}
}
