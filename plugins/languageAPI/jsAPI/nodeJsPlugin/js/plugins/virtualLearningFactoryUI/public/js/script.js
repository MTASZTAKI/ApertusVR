var apiEndPoint = 'http://localhost:3000/api/v1/';
var userNodeName;
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

function getUserNodeName() {
	console.log('getUserNodeName()');
	doGetRequest(apiEndPoint + 'userNodeName/', function (res) {
		userNodeName = res.data.items[0].name;
		console.log('userNodeName(): res: ', userNodeName);
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

function attachUserNodes() {
	console.log('attachUserNodes()');
	otherUserNodeNames.forEach(function (element) {
		doGetRequest(apiEndPoint + "/nodes/" + element.name + "/" + userNodeName + '/parent', function (res) {
			var parentNodeName = res.data.items[0].parentName;
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
	document.getElementById('otherUserNodeNames').innerHTML = '';
	otherUserNodeNames.forEach(function (element) {
		document.getElementById('otherUserNodeNames').innerHTML = document.getElementById('otherUserNodeNames').innerHTML  + element.name;
	});
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
	var userNodeNameDiv = document.getElementById(userNodeName);
	if (typeof (userNodeNameDiv) != 'undefined' && userNodeNameDiv != null) {
		userNodeNameDiv.innerHTML = userNodeName + ': Position(' + userNodePostion.x + ',' + userNodePostion.y + ',' + userNodePostion.z + ')';
	}
	else
	{
		var newDiv = document.createElement(userNodeName);
		newDiv.id = userNodeName;
		newDiv.innerHTML = userNodeName + ': Position(' + userNodePostion.x + ',' + userNodePostion.y + ',' + userNodePostion.z + ')';
		mapDiv.appendChild(newDiv);
	}
	let index = 0
	otherUserNodeNames.forEach(function (element) {
		var otherUserNodeNameDiv = document.getElementById(element.name);
		if (typeof (otherUserNodeNameDiv) != 'undefined' && otherUserNodeNameDiv != null) {
			otherUserNodeNameDiv.innerHTML = element.name + ': Position(' + otherUserNodePositions[index].x + ',' + otherUserNodePositions[index].y + ',' + otherUserNodePositions[index].z + ')';
			ctx.beginPath();
			ctx.arc(95, 10, 10, 0, 2 * Math.PI);
			ctx.stroke();
		}
		else {
			var newDiv = document.createElement(element.name);
			newDiv.id = element.name;
			newDiv.innerHTML = element.name  + ': Position(' + otherUserNodePositions[index].x + ',' + otherUserNodePositions[index].y + ',' + otherUserNodePositions[index].z + ')';
			mapDiv.appendChild(newDiv);
		}
		index++;
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
