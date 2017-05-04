var config_manager = require('../../helpers/config_manager/config_manager');


exports.sourcePath = config_manager.paths.source.apePath + config_manager.paths.source.jsApiPath;
exports.nodeModulesPath = config_manager.paths.build.binPath + config_manager.paths.build.configurationPath + config_manager.paths.build.nodeModulesPath;
exports.apertusModulePath = config_manager.paths.nodeModulesPath + config_manager.paths.build.apertusModulePath;

exports.requireNodeModule = function(moduleName) {
	return require(this.nodeModulesPath + moduleName);
}
