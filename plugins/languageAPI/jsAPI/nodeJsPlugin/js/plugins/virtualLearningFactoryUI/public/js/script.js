var apiEndPoint = 'http://localhost:3000/api/v1/';
var userNodeName = '';

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

function showMap() {
    console.log('toogle map');
	$('#map').toggle();
	getUserNodeName();
    document.getElementById('UserNodeName').innerHTML = userNodeName;
}

$(document).ready(function(){
	$('#map').toggle();
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
