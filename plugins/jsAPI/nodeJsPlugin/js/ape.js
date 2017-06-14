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

var moduleManager = require('./modules/module_manager/module_manager.js');
var express = moduleManager.requireNodeModule('express');
var utils = require('./modules/utils/utils.js');
var logger = require("./modules/logger/logger.js");

exports.nbind = require(moduleManager.apertusModulePath + 'nbind.node');
exports.pluginLoader = require('./modules/pluginLoader/pluginLoader.js');

exports.initApi = function(app) {
	var router = express.Router();
	router.use(require(moduleManager.sourcePath + 'api/index.js'));
	router.use(require(moduleManager.sourcePath + 'api/node.js'));
	router.use(require(moduleManager.sourcePath + 'api/light.js'));
	router.use(require(moduleManager.sourcePath + 'api/text.js'));

	router.use(require(moduleManager.sourcePath + 'api/fallback.js'));
	app.use('/api/v1', router);
}

exports.start = function(app) {
	this.initApi(app);

	// debugs
	logger.debug(utils.inspect(this));
	utils.iterate(this.nbind.JsBindManager(), 'ape.nbind.JsBindManager()', '');

	// start special plugins
	this.pluginLoader.loadPlugins();
};
