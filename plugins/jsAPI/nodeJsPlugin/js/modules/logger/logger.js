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

var moduleManager = require('../../modules/module_manager/module_manager.js');
var winston = moduleManager.requireNodeModule('winston');
var config_manager = require('../../modules/config_manager/config_manager');

winston.emitErrs = true;

const env = process.env.NODE_ENV || 'development';

// const tsFormat = () => (new Date()).toLocaleTimeString();
var logger = new winston.Logger({
	transports: [
		// colorize the output to the console
		new winston.transports.Console(config_manager.logger.console),
		new(moduleManager.requireNodeModule('winston-daily-rotate-file'))(config_manager.logger.file)
	],
	exitOnError: config_manager.logger.exitOnError
});

module.exports = logger;
module.exports.stream = {
	write: function(message, encoding) {
		logger.info(message);
	}
};
