//var nbind = require('./build/Release/nbind');
var nbind = require('nbind');
var util = require('util');

var moduleTag = 'JS: ';

String.prototype.contains = function(it) { return this.indexOf(it) != -1; };

String.prototype.replaceAll = function(search, replacement) {
	var target = this;
	return target.replace(new RegExp(search, 'g'), replacement);
};

function nameOf (exp) {
	return exp.toString().match(/[.](\w+)/)[1];
};

function log(tag, str) {
	console.log(moduleTag + ': ' + tag + ': ' + str);
}

function iterate(obj, nameOfObj, stack) {
	console.log('');
	console.log('[' + nameOfObj + ']');
    for (var property in obj) {
    	//console.log('prop: ' + obj);
    	var fnName = obj[property].toString();
    	if (fnName.contains('function')) {
    		fnName = fnName.replaceAll('function ', '');
    		fnName = fnName.substring(0, fnName.indexOf(' '));

    		// if (fnName != 'free()') {
	    	// 	var str = 'obj.' + fnName;
	    	// 	eval('(' + str + ')');
    		// }

    		console.log(' - ' + fnName);
    	}
        // if (obj.hasOwnProperty(property)) {
        // 	console.log('prop: ' + obj[property]);
        //     if (typeof obj[property] == "object") {
        //         iterate(obj[property], stack + '.' + property);
        //     } else {
        //         console.log(property + "   " + obj[property]);
        //         $('#output').append($("<div/>").text(stack + '.' + property))
        //     }
        // }
    }
    console.log('');
}

nbind.init('./build/Release/', function(err, binding) {
	if (err) {
		console.log(err);
		return;
	}

    var lib = binding.lib;
    console.log(lib);

    // greeter class
    var gre = lib.Greeter();
    console.log(gre.sayHello('you'));


    // classexample
    var example1 = lib.ClassExample();
    // var example2 = lib.ClassExample(3, 7);
    // var example2 = lib.ClassExample('test message');
    
    
    // ApeColor tests
    var apeColor1 = lib.Color();
    iterate(apeColor1, 'apeColor1', '');
    console.log('apeColor1 toString(): ' + apeColor1.toString());
    console.log('apeColor1 r: ' + apeColor1.r + ' g: ' + apeColor1.g + ' b: ' + apeColor1.b + ' a: ' + apeColor1.a);

    var apeColor2 = lib.Color(1, 2, 3, 4);
    iterate(apeColor2, 'apeColor2', '');
    console.log('apeColor2 toString(): ' + apeColor2.toString());
    console.log('apeColor2 r: ' + apeColor2.r + ' g: ' + apeColor2.g + ' b: ' + apeColor2.b + ' a: ' + apeColor2.a);
    

    // NodeImpl test
    // var node1 = lib.NodeImp();
    // console.log(node1);
    // console.log(node.getPosition());

    var nodeExample = lib.NodeExample();
    iterate(nodeExample, 'nodeExample', '');


    //var nObj = lib.NodeJsPtr(nodeExample.createNode('testNode2'));

    var nodeJsObj = nodeExample.createNode('testNode');
    iterate(nodeJsObj, 'nodeJsObj', '');
    console.log('nodeJsObj getName(): ' + nodeJsObj.getName());
    nodeJsObj.setPosition(lib.Vector3(2, 3, 4));
    console.log('nodeJsObj getPosition(): ' + nodeJsObj.getPosition().toString());
    console.log('nodeJsObj getDerivedPosition(): ' + nodeJsObj.getDerivedPosition().toString());
    console.log('nodeJsObj getOrientation(): ' + nodeJsObj.getOrientation().toString());
    console.log('nodeJsObj getDerivedOrientation(): ' + nodeJsObj.getDerivedOrientation().toString());
    console.log('nodeJsObj getScale(): ' + nodeJsObj.getScale().toString());
    console.log('nodeJsObj getDerivedScale(): ' + nodeJsObj.getDerivedScale().toString());
    

    // var nodeJsObjPos = nodeJsObj.getPosition();
    // console.log('nodeJsObj getPosition(): x: ' + nodeJsObjPos.x + ' y: ' + nodeJsObjPos.y + ' z: ' + nodeJsObjPos.z);



    var lightObj = nodeExample.createLight('testLight');
    lightObj.setDiffuseColor(apeColor2);
    var lightColor = lightObj.getDiffuseColor();
    console.log('light color from JS: ' + lightColor.r + ' g: ' + lightColor.g + ' b: ' + lightColor.b + ' a: ' + lightColor.a);


    var quaternionObj = lib.Quaternion(5, 6, 7, 8);
    iterate(quaternionObj, 'quaternionObj', '');

    console.log('end');
});
