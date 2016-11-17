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


const async = require('async');
const sequelize_manager = require('./sequelize_manager');
const utils = require('./utils');

exports.list = function(req, res) {
	console.log('sessions.list()');

	var queryStr ='SELECT * FROM sessions';
	sequelize_manager.handleQuery(req, res, queryStr, {type: sequelize_manager.sequelize.QueryTypes.SELECT});
};

exports.create = function(req, res) {
	console.log('sessions.create()');

	req.checkBody('sessionName', 'sessionName is not presented or empty').notEmpty();
	req.checkBody('sessionGuid', 'sessionGuid is not presented or empty').notEmpty();
	if (!sequelize_manager.checkErrors(req, res)) {
		var queryStr = 'CALL addSession(\'' + req.body.sessionName + '\', \'' + req.body.sessionGuid + '\');';
		sequelize_manager.handleQuery(req, res, queryStr);
	}
};

exports.read = function(req, res) {
	console.log('sessions.read()');

	req.checkParams('sessionName', 'UrlParam is not presented or empty').notEmpty();
	if (!sequelize_manager.checkErrors(req, res)) {
		var queryStr = 'CALL getSession(\'' + req.params.sessionName + '\');';
		sequelize_manager.handleQuery(req, res, queryStr);
	}
};

exports.update = function(req, res) {
	console.log('sessions.update()');
	console.log('Not Implemented');

	// TODO: implement
	res.send('Not Implemented');
};

exports.delete = function(req, res) {
	console.log('sessions.delete()');

	req.checkParams('sessionName', 'UrlParam is not presented or empty').notEmpty();
	if (!sequelize_manager.checkErrors(req, res)) {
		var queryStr = 'CALL removeSession(\'' + req.params.sessionName + '\');';
		sequelize_manager.handleQuery(req, res, queryStr);
	}
};
