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

var ape = require('../../ape.js');
var moduleManager = require('../../modules/module_manager/module_manager.js');
var async = moduleManager.requireNodeModule('async');
var logger = require("../../modules/log_manager/log_manager.js");
var config = require('./config.json');

exports.moduleTag = 'PluginManager';

exports.loadPlugins = function() {
	logger.debug("PluginManager::loadPlugins()");

	var configFolderPath = ape.nbind.JsBindManager().getFolderPath();
	logger.debug('PluginManager::loadPlugins() configFolderPath: ' + configFolderPath);

	var config = require(configFolderPath + '\\ApeNodeJsPlugin.json');
	logger.debug(JSON.stringify(config.pluginManager.plugins));
	logger.debug('PluginManager::loadPlugins() plugins to start:');
	for (var i = 0; i < config.pluginManager.plugins.length; i++) {
		logger.debug(' - ', config.pluginManager.plugins[i].file);
	}

	var q = async.queue(function(plugin, callback) {
		logger.debug(plugin.file + ' init function called');
		var pluginFilePath = moduleManager.pluginPath + plugin.file;
		var pluginInstance = require(pluginFilePath);
		pluginInstance.init(plugin.args);

		callback();
	}, config.pluginManager.plugins.length);

	q.drain = function() {
		logger.debug('PluginManager::loadPlugins() all plugins have been started');
	};

	for (var i = 0; i < config.pluginManager.plugins.length; i++) {
		q.push(config.pluginManager.plugins[i]);
	}
}
