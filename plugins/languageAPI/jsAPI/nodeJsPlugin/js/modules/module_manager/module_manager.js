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

var config = require('./config.json');
var fs = require('fs');

exports.configType = '';

exports.sourcePath = config.source.apePath + config.source.jsApiPath;
exports.pluginPath = this.sourcePath + 'plugins/';
exports.nodeModulesPath = config.build.binPath + this.configType + config.build.nodeModulesPath;
exports.apertusModulePath = this.nodeModulesPath + config.build.apertusModulePath;
exports.httpServer = {
	host: "",
	port: 0,
	address: ""
};

exports.requireNodeModule = function(moduleName) {
	if (!fs.existsSync(this.nodeModulesPath + moduleName)) {
		console.log('error: required module does not exist: "' + this.nodeModulesPath + moduleName + '"');
		console.log('config.source.apePath: "' + config.source.apePath + '"');
		console.log('config.build.binPath: "' + config.build.binPath + '"');
		console.log('configuration:');
		console.log(config);
		process.exit(1);
	}
	return require(this.nodeModulesPath + moduleName);
}

exports.setConfigType = function(configStr) {
	exports.configType = configStr + '/';
	exports.configurationPath = config.build.binPath + this.configType;
	exports.nodeModulesPath = config.build.binPath + this.configType + config.build.nodeModulesPath;
	exports.apertusModulePath = this.nodeModulesPath + config.build.apertusModulePath;
}
