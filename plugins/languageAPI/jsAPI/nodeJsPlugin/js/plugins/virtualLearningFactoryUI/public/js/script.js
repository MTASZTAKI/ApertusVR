var apiEndPoint = 'http://localhost:3000/api/v1/';
var userNodeName;
var userNodePostion;
var otherUserNodeNames;
var otherUserNodePositions;

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

function getUserNodeName() {
	console.log('getUserNodeName()');
	doGetRequest(apiEndPoint + 'userNodeName/', function (res) {
		userNodeName = res.data.items[0].name;
		console.log('userNodeName(): res: ', userNodeName);
	});
}

function getUserNodePosition() {
	console.log('getUserNodePosition()');
	doGetRequest(apiEndPoint + userNodeName + '/position', function (res) {
		userNodePostion = res.data.items[0].position;
		console.log('getUserNodePosition(): res: ', userNodePostion);
		$('#posX').val(userNodePostion.x);
		$('#posY').val(userNodePostion.y);
		$('#posZ').val(userNodePostion.z);
	});
}

function getOtherUserNodeNames() {
	console.log('getOtherUserNodeNames()');
	doGetRequest(apiEndPoint + 'otherUserNodeNames/', function (res) {
		var userNodeNames = res.data.items;
		userNodeNames.forEach(function (element) {
			otherUserNodeNames.push(element.name);
		});
	});
}

function getOtherUserNodePositions() {
	console.log('attachUserNodes()');
	otherUserNodeNames.forEach(function (element) {
		doGetRequest(apiEndPoint + element.name + '/position', function (res) {
			var otherUserNodePosition = res.data.items[0].position;
			console.log('getOtherUserNodePositions(): res: ', otherUserNodePosition);
			$('#posX').val(otherUserNodePosition.x);
			$('#posY').val(otherUserNodePosition.y);
			$('#posZ').val(otherUserNodePosition.z);
			otherUserNodePositions.push(otherUserNodePosition);
		});
	});
}

function attachUserNodes() {
	console.log('attachUserNodes()');
	otherUserNodeNames.forEach(function (element) {
		doGetRequest(apiEndPoint + element.name + '/attach', userNodeName, function (res) {
			parentNodeName = res.data.items[0].name;
			console.log('parentNodeName(): res: ', parentNodeName);
		});
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
	getUserNodeName();
	getOtherUserNodeNames();
	document.getElementById('otherUserNodeNames').innerHTML = otherUserNodeNames;
	attachUserNodes();
}

function updateMap() {
	console.log('update map');
	getUserNodeName();
	getOtherUserNodeNames();
	getUserNodePosition();
	getOtherUserNodePositions();

	var canvas = document.getElementById("mapCanvas");
	var ctx = canvas.getContext("2d");
	var mapDiv = document.getElementById('map');
	if (!document.getElementById(userNodeName)) {
		var newDiv = document.createElement(userNodeName);
		newDiv.innerHTML = userNodePostion;
		mapDiv.appendChild(newDiv);
	}
	else
	{
		document.getElementById(userNodeName).innerHTML = userNodePostion;
	}
	userNodeNames.forEach(function (element) {
		if (!document.getElementById(element.name)) {
			var newDiv = document.createElement(element.name);
			newDiv.innerHTML = otherUserNodePositions[element.id];
			mapDiv.appendChild(newDiv);
		}
		else {
			document.getElementById(element.name).innerHTML = otherUserNodePositions[element.id];
			ctx.beginPath();
			ctx.arc(95, 10, 10, 0, 2 * Math.PI);
			ctx.stroke();
		}
	});
}

function showMap() {
    console.log('toogle map');
    $('#map').toggle();
    updateMap()
}

$(document).ready(function(){
	$('#map').toggle();
	$('#users').toggle();
    var sock = new WebSocket("ws://localhost:40080/ws");
    sock.onopen = ()=>{
        console.log('open')
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
		
		if (eventObj.type == 9) { 
			$("#nodeName").val(eventObj.subjectName);
            nodeName = eventObj.subjectName;
			console.log(' show bounding box - select: ', nodeName);
			document.getElementById('selectedNodeNameTitle').innerHTML = nodeName;
        }
    }
});
