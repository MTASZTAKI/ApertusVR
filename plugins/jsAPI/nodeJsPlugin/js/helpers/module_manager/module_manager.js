

exports.nodeModulesPath = 'c:/Users/User/dev/repos/ape/v3/ApertusVR-build/bin/debug/node_modules/';
exports.apertusModulePath = this.nodeModulesPath + 'apertusvr/';
exports.sourcePath = 'c:/Users/User/dev/repos/ape/v3/ApertusVR/plugins/jsAPI/nodeJsPlugin/js/';

exports.requireNodeModule = function(moduleName) {
	return require(this.nodeModulesPath + moduleName);
}
