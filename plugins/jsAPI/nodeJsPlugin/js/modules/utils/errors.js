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

exports.setMessage = function(name, msg) {
	if (this.items.hasOwnProperty(name))
		utils.items[name].message = msg;
}

exports.getByName = function(name) {
	return utils.items[name];
}

exports.items = {
	UnknownError: {
		name: 'UnknownError',
		title: 'Error #0',
		message: 'Unknown error happened. :(',
		code: 0,
		httpStatusCode: 404
	},
	validationErrors: {
		name: 'validationErrors',
		title: 'Error #1',
		message: 'Some HTTP params are not presented or cannot be validated.',
		code: 1,
		httpStatusCode: 1
	},
	undefinedVariable: {
		name: 'undefinedVariable',
		title: 'Error #2',
		message: 'Variable is undefined.',
		code: 2,
		httpStatusCode: 2
	},
	dataNotPresented: {
		name: 'dataNotPresented',
		title: 'Error #3',
		message: 'Data object is not presented.',
		code: 3,
		httpStatusCode: 3
	},
	resourceNotFound: {
		name: 'resourceNotFound',
		title: 'Error #4',
		message: 'Resource not found under this URL.',
		code: 4,
		httpStatusCode: 404
	},
	noToken: {
		name: 'noToken',
		title: 'Error #5',
		message: 'No token provided.',
		code: 5,
		httpStatusCode: 401
	},
	invalidToken: {
		name: 'invalidToken',
		title: 'Error #6',
		message: 'Invalid token provided.',
		code: 6,
		httpStatusCode: 401
	},
	zeroLength: {
		name: 'zeroLength',
		title: 'Error #7',
		message: 'Data has zero length.',
		code: 7,
		httpStatusCode: 7
	},
	noTokenPayload: {
		name: 'noTokenPayload',
		title: 'Error #8',
		message: 'No token payload provided.',
		code: 8,
		httpStatusCode: 401
	},
	UnauthorizedError: {
		name: 'UnauthorizedError',
		title: 'Error #9',
		message: 'Unauthorized token provided.',
		code: 9,
		httpStatusCode: 401
	},
	ReferenceError: {
		name: 'ReferenceError',
		title: 'Error #10',
		message: 'ReferenceError.',
		code: 10,
		httpStatusCode: 404
	},
	ConnectionRefused: {
		name: 'ConnectionRefused',
		title: 'Error #11',
		message: 'Connection refused by server.',
		code: 11,
		httpStatusCode: 404
	},
	ServerIsNotResponding: {
		name: 'ServerIsNotResponding',
		title: 'Error #12',
		message: 'Cannot connect to the server.',
		code: 12,
		httpStatusCode: 404
	},
	LoginError: {
		name: 'LoginError',
		title: 'Error #13',
		message: 'Invalid username or password.',
		code: 13,
		httpStatusCode: -1
	}
}
