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

var config_manager = require('../../helpers/config_manager/config_manager');

exports.configType = '';

exports.sourcePath = config_manager.paths.source.apePath + config_manager.paths.source.jsApiPath;
exports.nodeModulesPath = config_manager.paths.build.binPath + this.configType + config_manager.paths.build.nodeModulesPath;
exports.apertusModulePath = this.nodeModulesPath + config_manager.paths.build.apertusModulePath;

exports.requireNodeModule = function(moduleName) {
	return require(this.nodeModulesPath + moduleName);
}

exports.setConfigType = function(configStr) {
	//this.configType = configStr + '/';
	exports.configType = configStr + '/';
	exports.nodeModulesPath = config_manager.paths.build.binPath + this.configType + config_manager.paths.build.nodeModulesPath;
	exports.apertusModulePath = this.nodeModulesPath + config_manager.paths.build.apertusModulePath;
}
