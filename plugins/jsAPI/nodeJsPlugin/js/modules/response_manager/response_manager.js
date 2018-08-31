/*MIT License

Copyright (c) 2017 Akos Hamori

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

var utils = require('../../modules/utils/utils.js');
var errorMap = require('../../modules/utils/errors.js');

function Response(req) {
	this.clear();
	this.setUrl(req.protocol + '://' + req.get('host') + req.originalUrl);
	if (req.query) {
		this._schema.data.numItemsPerPage = Number(req.query.limit || 0);
		this._schema.data.numItemsFrom = Number(req.query.skip || 0);
		this._schema.data.numItemsTo = Number(this._schema.data.numItemsFrom) + Number(this._schema.data.numItemsPerPage);
	}
}

Response.prototype.getSchema = function() {
	return {
		result: {
			success: true,
			statuscode: 200,
			url: '',
			title: 'API',
			description: ''
		},
		token: {
			key: '',
			expires: ''
		},
		errors: {
			count: 0,
			items: []
		},
		events: {
			count: 0,
			items: []
		},
		data: {
			numItemsPerPage: 0,
			numItemsFrom: 0,
			numItemsTo: 0,
			numItemsLeft: 0,
			numItemsFullCount: 0,

			count: 0,
			items: []
		}
	};
}

Response.prototype.clear = function() {
	this._schema = this.getSchema();
}

Response.prototype.getSuccess = function() {
	return this._schema.result.success;
};

Response.prototype.setSuccess = function(value) {
	this._schema.result.success = value;
};

Response.prototype.getUrl = function() {
	return this._schema.result.url;
};

Response.prototype.setUrl = function(value) {
	this._schema.result.url = value;
};

Response.prototype.getStatusCode = function() {
	return this._schema.result.statuscode;
};

Response.prototype.setStatusCode = function(value) {
	this._schema.result.statuscode = value;
};

Response.prototype.getTitle = function() {
	return this._schema.result.title;
};

Response.prototype.setTitle = function(value) {
	this._schema.result.title = value;
};

Response.prototype.getDescription = function() {
	return this._schema.result.description;
};

Response.prototype.setDescription = function(value) {
	this._schema.result.description = value;
};

Response.prototype.validateHttpParams = function(req) {
	var errors = req.validationErrors();
	if (errors) {
		var errorObj = errorMap.items.validationErrors;
		errorObj.items = errors;
		this.addErrorItem(errorObj);
		return false;
	} else {
		return true;
	}
};

Response.prototype.addErrorItem = function(item) {
	this.setSuccess(false);

	if (!utils.isDefined(item)) {
		this.addErrorItem(errorMap.items.undefinedVariable);
		return;
	}

	this._schema.errors.items.push(item);
	this._schema.errors.count = this._schema.errors.items.length;

	if (item.hasOwnProperty('httpStatusCode')) {
		this.setStatusCode(item.httpStatusCode);
	}
};

Response.prototype.setErrorItems = function(items) {
	if (!utils.isDefined(items)) {
		this.addErrorItem(errorMap.items.undefinedVariable);
		return;
	}

	if (items.length == 0) {
		this.addErrorItem(errorMap.items.zeroLength);
		return;
	}

	this.setSuccess(false);
	this._schema.errors.items = items;
	this._schema.errors.count = this._schema.errors.items.length;
};

Response.prototype.addEventItem = function(item) {
	if (!utils.isDefined(item)) {
		this.addErrorItem(errorMap.items.undefinedVariable);
		return;
	}

	this._schema.events.items.push(item);
	this._schema.events.count = this._schema.events.items.length;
};

Response.prototype.setEventItems = function(items) {
	if (!utils.isDefined(items)) {
		this.addErrorItem(errorMap.items.undefinedVariable);
		return;
	}

	if (items.length == 0) {
		this.addErrorItem(errorMap.items.zeroLength);
		return;
	}

	this._schema.events.items = items;
	this._schema.events.count = this._schema.events.items.length;
};

Response.prototype.getDataItems = function(item) {
	return this._schema.data.items;
};

Response.prototype.addDataItem = function(item) {
	if (!utils.isDefined(item)) {
		this.addErrorItem(errorMap.items.undefinedVariable);
		return;
	}

	this._schema.data.items.push(item);
	this._schema.data.count = this._schema.data.items.length;
};

Response.prototype.setDataItems = function(items) {
	if (!utils.isDefined(items)) {
		this.addErrorItem(errorMap.items.undefinedVariable);
		return;
	}

	if (items.length == 0) {
		this.addErrorItem(errorMap.items.zeroLength);
		return;
	}

	this._schema.data.items = items;
	this._schema.data.count = this._schema.data.items.length;
};

Response.prototype.setToken = function(value) {
	if (!utils.isDefined(value)) {
		this.addErrorItem(errorMap.items.undefinedVariable);
		return;
	}

	this._schema.token.key = value;
};

Response.prototype.toJSonString = function() {
	var resp = this._schema;
	if (resp.token) {
		if (!resp.token.key && !resp.token.expires) {
			// delete resp.token;
		}
	}
	if (resp.errors) {
	 	if (resp.errors.count == 0) {
			// delete resp.errors;
		}
	}
	if (resp.events) {
		if (resp.events.count == 0) {
			// delete resp.events;
		}
	}
	if (resp.data) {
	 	if (resp.data.count == 0) {
			// delete resp.data;
		}
	}
	return resp;
};

module.exports = Response;
