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

var utils = require('../../modules/utils/utils.js');
var ape = require('../../ape.js');

exports.init = function() {
	// if mDemoObjectNode found, attach a textGeometry and set caption
	ape.nbind.JsBindManager().getNode('mDemoObjectNode', function(error, obj) {
		if (error) {
			console.log('error: ' + error);
			return;
		}
		var textObj = ape.nbind.JsBindManager().createText('textGeometry');
		textObj.setParentNodeJsPtr(obj);

		setInterval(function() {
			var pos = obj.getPosition();
			var ort = obj.getOrientation();
			textObj.setCaption('x: ' + utils.roundDecimal(pos.x) + ', y: ' + utils.roundDecimal(pos.y) + ', z: ' + utils.roundDecimal(pos.z) + '\n' +
				'w: ' + utils.roundDecimal(ort.w) + ', x: ' + utils.roundDecimal(ort.x) + ', y: ' + utils.roundDecimal(ort.y) + ', z: ' + utils.roundDecimal(ort.z)
			);
		}, 20);
	});
}
