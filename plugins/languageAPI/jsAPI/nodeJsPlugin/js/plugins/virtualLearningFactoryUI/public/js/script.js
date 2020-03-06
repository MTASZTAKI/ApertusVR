var apiEndPoint = 'http://localhost:3000/api/v1/';
var userNodeName;
var userID;
var userNodePostion;
var otherUserNodeNames = [];
var otherUserNodePositions = [];

function getNodesNames() {
	console.log('getNodesNames()');
	doGetRequest(apiEndPoint + 'nodeNames/', function (res) {
		var nodeNames = res.data.items;
		console.log('getNodesNames(): res: ', nodeNames);
		nodeNames.forEach(function (element) {
			genNodeItem(element.name);
		});
	});
}

function getUserNodeNameAndID() {
	console.log('getUserNodeNameAndID()');
	doGetRequest(apiEndPoint + 'userNodeName/', function (res) {
		userNodeName = res.data.items[0].name;
		userID = res.data.items[1].ownerID;
		console.log('userNodeName(): res: ', userNodeName);
		console.log('userID(): res: ', userID);
	});
}

function getUserNodePosition() {
	console.log('getUserNodePosition()');
	doGetRequest(apiEndPoint + "/nodes/" + userNodeName + '/position', function (res) {
		userNodePostion = res.data.items[0].position;
		console.log('getUserNodePosition(): res: ', userNodePostion);
	});
}

function getOtherUserNodeNames() {
	console.log('getOtherUserNodeNames()');
	doGetRequest(apiEndPoint + 'otherUserNodeNames/', function (res) {
		otherUserNodeNames = res.data.items;
	});
}

function getOtherUserNodePositions() {
	console.log('getOtherUserNodePositions()');
	otherUserNodeNames.forEach(function (element) {
		doGetRequest(apiEndPoint + "/nodes/" + element.name + '/position', function (res) {
			console.log('getOtherUserNodePositions(): res: ', res.data.items[0].position);
			otherUserNodePositions.push(res.data.items[0].position);
		});
	});
}

function attachOtherUserNode2Me(otherUserNodeName) {
	console.log('try to attachOtherUserNode2Me: ', otherUserNodeName);
	doGetRequest(apiEndPoint + "/nodes/" + otherUserNodeName + "/" + userNodeName + '/parent', function (res) {
		var currentParentNodeName = res.data.items[0].parentName;
		console.log('get parentNodeName(): currentParentNodeName: ', currentParentNodeName);
		if (currentParentNodeName == userNodeName) {
			doPostRequest(apiEndPoint + "/nodes/" + otherUserNodeName + "/" + "root" + '/parent', function (res) {
				var parentNodeName = res.data.items[0].parentName;
				console.log('post setParent() zero: res: ', parentNodeName);
				$('#users').toggle();
			});
		}
		else {
			doPostRequest(apiEndPoint + "/nodes/" + otherUserNodeName + "/" + userID + '/owner', function (res) {
				var ownerID = res.data.items[0].ownerID;
				console.log('post setOwner() res: ', ownerID);
				doPostRequest(apiEndPoint + "/nodes/" + otherUserNodeName + "/" + userNodeName + '/parent', function (res) {
					var parentNodeName = res.data.items[0].parentName;
					console.log('post setParent(): res: ', parentNodeName);
					$('#users').toggle();
				});
			});
		}
	});
}

function setNodePosition(nodeName, pos) {
	console.log('setNodePosition(): ', nodeName, ' pos: ', pos);
	doPostRequest(apiEndPoint + "/nodes/" + nodeName + '/position', pos, function (res) {
		console.log('setNodePosition(): res: ', res);
	});
}

function setNodeOrientation(nodeName, ori) {
	console.log('setNodeOrientation(): ', nodeName, ' ori: ', ori);
	doPostRequest(apiEndPoint + "/nodes/" + nodeName + '/orientation', ori, function (res) {
		console.log('setNodeOrientation(): res: ', res);
	});
}

function doGetRequest(apiEndPointUrl, callback) {
	console.log('doGetRequest()');
    $.get(apiEndPointUrl, function(res) {
        console.log('doGetRequest(): res: ', res);
        callback(res);
    });
}

function doPostRequest(apiEndPointUrl, data, callback) {
    $.post(apiEndPointUrl, data, function(res) {
        // console.log('doPostRequest(): ', res);
        callback(res);
    }, "json");
}

function showChat() {
    console.log('toogle chat');
	$('#chat').toggle();
}

function showUsers() {
	console.log('toogle users');
	$('#users').toggle();
	getUserNodeNameAndID();
	getOtherUserNodeNames();
	var usersDiv = document.getElementById('users');
	otherUserNodeNames.forEach(function (element) {
		var otherUserNodeNameDiv = document.getElementById(element.name);
		if (typeof (otherUserNodeNameDiv) != 'undefined' && otherUserNodeNameDiv != null) {
			otherUserNodeNameDiv.innerHTML = element.name;
		}
		else {
			var newDiv = document.createElement('div');
			newDiv.id = element.name;
			newDiv.innerHTML = element.name;
			newDiv.addEventListener('click', function () {
				var pos = {x: 0, y: 0, z: 0};
				setNodePosition(element.name, pos);
				var ori = {w: 1, x: 0, y: 0, z: 0};
				setNodeOrientation(element.name, ori);
				attachOtherUserNode2Me(element.name);
			});
			usersDiv.appendChild(newDiv);
		}
	});
}

function updateMap() {
	console.log('update map');
	getUserNodeNameAndID();
	getOtherUserNodeNames();
	getUserNodePosition();
	getOtherUserNodePositions();

	var canvas = document.getElementById("mapCanvas");
	var ctx = canvas.getContext("2d");
	ctx.clearRect(0, 0, canvas.width, canvas.height);

	var mapDiv = document.getElementById('map');
	
	let index = 0;
	otherUserNodeNames.forEach(function (element) {
		var scale = 0.1;
		var pos = otherUserNodePositions.pop();
		var relativePosition = { x: (userNodePostion.x - pos.x) * scale, y: (userNodePostion.y - pos.y) * scale, z: (userNodePostion.z - pos.z) * scale };
		console.log('relativePosition: ', relativePosition)
		ctx.beginPath();
		ctx.arc((canvas.width / 2) - relativePosition.x, (canvas.height / 2) - relativePosition.y, 5, 0, 2 * Math.PI);
		ctx.stroke();
		index++;
	});
}

function showMap() {
    console.log('toogle map');
    $('#map').toggle();
}

$(document).ready(function () {
	getUserNodeNameAndID();
	getOtherUserNodeNames();
	getUserNodePosition();
	getOtherUserNodePositions();
	$('#map').toggle();
	$('#users').toggle();
    var sock = new WebSocket("ws://localhost:40080/ws");
    sock.onopen = ()=>{
    	console.log('open')
    	window.setInterval(function () {
    		//updateMap();
    	}, 500);
    }
    sock.onerror = (e)=>{
        console.log('error',e)
    }
    sock.onclose = ()=>{
        console.log('close')
    }
    sock.onmessage = (e)=>{
        console.log('onmessage:' + e.data);
        var eventObj = JSON.parse(e.data);
        console.log('eventObj: ', eventObj);
		
		if (eventObj.type == 10) { 
			$("#nodeName").val(eventObj.subjectName);
            nodeName = eventObj.subjectName;
			console.log(' show bounding box - select: ', nodeName);
			document.getElementById('selectedNodeNameTitle').innerHTML = nodeName;

			/*console.log('try to attach server created node to me: ', nodeName);
			doPostRequest(apiEndPoint + "/nodes/" + nodeName + "/" + userID + '/owner', function (res) {
				var ownerID = res.data.items[0].ownerID;
				console.log('post setOwner() res: ', ownerID);
				doPostRequest(apiEndPoint + "/nodes/" + nodeName + "/" + userNodeName + '/parent', function (res) {
					var parentNodeName = res.data.items[0].parentName;
					console.log('post attachNodeToMe res: ', parentNodeName);
				});
			});*/
        }
    }
});
