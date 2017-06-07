var yargs = require('yargs');
var shell = require('shelljs');
var colors = require('colors/safe');

yargs
	.usage('$0 <cmd> [args]')
	.command('3rdparty [command] [name]', 'welcome ter yargs!', {
			command: {
				default: 'default install command'
			},
			name: {
				default: 'default name'
			}
		},
		function (argv) {
			console.log('3rdParty: ' + argv.command + ' ' + argv.name + ' welcome to yargs!');
			if (argv.command == 'install') {
				console.log('Installing ' + colors.green(argv.name) + '...');
			}
		})
	.command('ls', '', {},
		function (argv) {
			console.log('shell: ls');
			shell.ls('*.*').forEach(function (file) {
				console.log(file);
			});
		})
	.help()
	.argv
