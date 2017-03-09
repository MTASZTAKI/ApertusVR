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

var utils = this;
var util = require('util');


String.prototype.isEmpty = function() {
    return (this.length === 0 || !this.trim());
};

String.prototype.contains = function(it) {
    return this.indexOf(it) != -1;
};

String.prototype.replaceAll = function(search, replacement) {
  var target = this;
  return target.replace(new RegExp(search, 'g'), replacement);
};

Array.prototype.pushArray = function(arr) {
    this.push.apply(this, arr);
};

exports.roundDecimal = function(num) {
  return Math.round(num * 100) / 100;
};

exports.isDefined = function(obj) {
	return (typeof obj !== 'undefined');
};

exports.isUndefined = function(obj) {
	return !this.isDefined(obj);
};

exports.log = function(tag, str) {
  console.log((this.moduleTag || '? ') + ': ' + tag + ': ' + str);
};

exports.inspect = function(obj) {
  return util.inspect(obj, {depth: null}); // null = no depth limit
};

exports.nameOf = function(exp) {
  return exp.toString().match(/[.](\w+)/)[1];
};

exports.iterate = function(obj, nameOfObj, stack) {
  console.log('');
  console.log('[' + nameOfObj + ']');
    for (var property in obj) {
      var fnName = obj[property].toString();
      if (fnName.contains('function')) {
        fnName = fnName.replaceAll('function ', '');
        fnName = fnName.substring(0, fnName.indexOf(' '));
        console.log(' - ' + fnName);
      }
    }
    console.log('');
};

exports.ObjToSource = function(o) {
    if (!o) return 'null';
    if (typeof(o) == "object") {
        if (!this.ObjToSource.check) this.ObjToSource.check = new Array();
        for (var i = 0, k = this.ObjToSource.check.length ; i < k ; ++i) {
            if (this.ObjToSource.check[i] == o) {return '{}';}
        }
        this.ObjToSource.check.push(o);
    }
    var k = "", na = typeof(o.length) == "undefined" ? 1 : 0, str = "";
    for(var p in o) {
        if (na) k = '"' + p + '":';
        if (typeof o[p] == "string") {
          var val = o[p];
          str += k + '"' + val + '",';
        }
        else if (typeof o[p] == "object") str += k + this.ObjToSource(o[p]) + ",\n";
        else {
          var val = o[p] + ' ';
          if (val.indexOf('function') > -1) {
            //val = 'function()';
            //str += k + '"' + val + '",';
          }
          else {
              str += k + '"' + val + '",';
          }
        }
    }
    if (typeof(o) == "object") this.ObjToSource.check.pop();
    if (na) return "{" + str.slice(0, -1) + "}\n";
    else return "[" + str.slice(0, -1) + "]\n";
};

exports.convertToJsObj = function(obj) {
  return JSON.parse(utils.ObjToSource(obj));
}

exports.responseObj = function() {
	return {
		response: {
			result: 'success',
			errors: {
				count: 0,
				items: []
			},
			events: {
				count: 0,
				items: []
			},
			data: {
				count: 0,
				items: []
			}
		},

		validateHttpParams: function(req, res) {
			var errors = req.validationErrors();
			if (errors) {
				var errorObj = {
					name: 'validationErrors',
					msg: 'Some HTTP params are not presented or cannot be validated.',
					code: 444,
					validationErrors: errors
				};
				//console.log(this);
				this.addError(errorObj);
				return false;
			}
			else {
				return true;
			}
		},
		addError: function(errorObj) {
			this.response.result = 'error';

			if (utils.isUndefined(errorObj)) {
				this.addError({name: 'undefinedVariable', msg: 'variable errorObj is undefined', code: 666});
				return;
			}

			this.response.errors.items.push(errorObj);
			this.response.errors.count = this.response.errors.items.length;
		},
		setErrorItems: function(items) {
			if (items) {
				this.response.errors.items = items;
				this.response.errors.count = this.response.errors.items.length;
				this.response.result = 'error';
			}
		},

		addEvent: function(event) {
			if (event) {
				this.response.events.items.push(event);
				this.response.events.count = this.response.events.items.length;
			}
		},
		setEventItems: function(items) {
			if (items) {
				this.response.events.items = items;
				this.response.events.count = this.response.events.items.length;
			}
		},

		setData: function(data) {
			if (data) {
				this.response.data = data;
			}
		},
		setDataItems: function(items) {
			if (items) {
				this.response.data.items = items;
				this.response.data.count = this.response.data.items.length;
			}
		},

		toJSonString: function() {
			if (this.response.errors.hasOwnProperty('count') && this.response.errors.count == 0) {
				delete this.response.errors;
			}
			if (this.response.events.hasOwnProperty('count') && this.response.events.count == 0) {
				delete this.response.events;
			}
			if (this.response.data.hasOwnProperty('count') && this.response.data.count == 0) {
				delete this.response.data;
			}

			//return JSON.stringify(this.response);
			return this.response;
		}
	}
};
