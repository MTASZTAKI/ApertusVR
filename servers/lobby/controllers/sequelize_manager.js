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


const Sequelize = require('sequelize');
const utils = require('./utils');
const sequelize = new Sequelize('apertus', 'root', 'apertusmysqlrootpassword', {
	host: 'localhost',
	dialect: 'mysql'
});

const sequelize_error_handler = this;

String.prototype.replaceAll = function(search, replacement) {
	var target = this;
	return target.replace(new RegExp(search, 'g'), replacement);
};

exports.sequelize = sequelize;


sequelize
  .authenticate()
  .then(function(err) {
    console.log('Connection has been established successfully.');
  })
  .catch(function (err) {
    console.log('Unable to connect to the database:', err);
  });

exports.catchError = function(req, res, error) {
	console.log('Error obj: ' + error);

	var result = {
		code: 400,
		message: ''
	};

	if (error.message) {
		var errorParts = error.message.split(':');
		if (errorParts.length == 0) {
			result.message = errorParts[0];
		}
		else {
			result.name = errorParts[0];
			result.message = errorParts[1];
		}
	}

	console.log('Error message: ' + result.message);
	return result;
};

exports.handleQuery = function(req, res, queryStr, queryOptions) {
	console.log('sequelize_manager.handleQuery(): queryStr: ' + queryStr);
	var result = new utils.response();
	if (queryOptions) {
		sequelize.query(queryStr, queryOptions)
			.then(function(data) {
				result.setData(data);
				res.send(result.data);
				return;
			})
			.error(function(error) {
				result.addError(error);
				res.send(result.data);
				return;
			})
			.catch(function(error) {
				result.addError(sequelize_error_handler.catchError(req, res, error));
				res.send(result.data);
				return;
			});
	}
	else {
		sequelize.query(queryStr)
			.then(function(data) {
				result.setData(data);
				res.send(result.data);
				return;
			})
			.error(function(error) {
				result.addError(error);
				res.send(result.data);
				return;
			})
			.catch(function(error) {
				result.addError(sequelize_error_handler.catchError(req, res, error));
				res.send(result.data);
				return;
			});
	}
};

exports.checkErrors = function(req, res) {
	var result = new utils.response();
	var errors = req.validationErrors();
	if (errors) {
		result.data.errors.items = errors;
		res.send(result.data);
		return true;
	}
	return false;
};
