var apiEndPoint = 'http://localhost:3000/api/v1/';
var apiEndPointNode = apiEndPoint + 'nodes/';


function getNodesNames() {
	console.log('getNodesNames()');
	doGetRequest(apiEndPointNode, function (res) {
		var nodeNames = res.data.items;
		console.log('getNodesNames(): res: ', nodeNames);

		nodeNames.forEach(function (element) {
			genNodeItem(element.name);
		});
	});
}

function doGetRequest(apiEndPointUrl, callback) {
    $.get(apiEndPointUrl, function(res) {
        // console.log('doGetRequest(): ', res);
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

$(document).ready(function(){
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
