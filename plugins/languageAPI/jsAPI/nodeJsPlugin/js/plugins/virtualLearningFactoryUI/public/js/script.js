var apiEndPoint = 'http://localhost:3000/api/v1/';
var userNodeName = '';
var userID;
var userNodePostion;
var otherUserNodeNames = [];
var otherUserNodePositions = [];
var clickedNodeName;
var clickedNodePosition;
var clickedNodeOrientation;
var clickedNodeDescr;
var roomName;
var animationJSON;
var sceneJSON;
var animationSpeedFactor = 1;
var isPaused = false;
var updateMapInterval;
var updatePropertiesInterval;
var updateAnimationTimeInterval;
var currentAnimationTime;
var updateOverlayBrowserLastMessageInterval;
var updateMeAttachedInterval;

function convertHex(hex, opacity) {
	hex = hex.replace('#', '');
	r = parseInt(hex.substring(0, 2), 16);
	g = parseInt(hex.substring(2, 4), 16);
	b = parseInt(hex.substring(4, 6), 16);
	result = 'rgba(' + r + ',' + g + ',' + b + ',' + opacity / 100 + ')';
	return result;
}

function updateMeAttached() {
	//console.log('updateMeAttached);
	var currentParentNodeName;
	doGetRequest(apiEndPoint + "/nodes/" + userNodeName + "/" + currentParentNodeName + '/parent', function (res) {
		currentParentNodeName = res.data.items[0].parentName;
		console.log('updateMeAttached: ', currentParentNodeName);
		if (currentParentNodeName.length) {
			$('#freeMe').show();
		}
		else {
			$('#freeMe').hide();
		}
	});
}

function getRoomName() {
	console.log('getRoomName()');
	doGetRequest(apiEndPoint + '/roomName', function (res) {
		roomName = res.data.items[0].roomName;
		console.log('getRoomName(): res: ', roomName);
		parseAnimationJSON();
		parseSceneJSON();
	});
}

function getOverlayBrowserLastMessage() {
	console.log('getOverlayBrowserLastMessage()');
	doGetRequest(apiEndPoint + '/overLayBrowserGetLastMessage', function (res) {
		lastMessage = res.data.items[0].lastMessage;
		console.log('getOverlayBrowserLastMessage(): res: ', lastMessage);
		updateAnimationTime(lastMessage);
		updateClickedNodeState(lastMessage);
	});
}

function updateClickedNodeState(time) {
	var ms = Math.floor((time / 100) % 10);
	var sec = Math.floor((time / 1000) % 60);
	if (ms == 0 && sec > 0) {
		animationJSON.nodes.forEach(function (node) {
			if (node.name == clickedNodeName) {
				element.actions.forEach(function (action) {
					if (action.event.type == "state" && action.trigger.type == "timestamp") {
						if (action.trigger.data == sec) {
							document.getElementById('selectedNodeState').innerHTML = 'State: ' + action.event.descr;
						}
					}
				});
			}
		});
	}
};

function updateAnimationTime(time) {
	var ms = Math.floor((time / 100) % 10);
	var sec = Math.floor((time / 1000) % 60);
	var min = Math.floor((time / (1000 * 60)) % 60);
	var hour = Math.floor((time / (1000 * 60 * 60)) % 24);
	var stopWatchDiv = document.getElementById('stopWatch');
	stopWatchDiv.innerHTML =
		(hour > 9 ? hour : "0" + hour) + ":" +
		(min > 9 ? min : "0" + min) + ":" +
		(sec > 9 ? sec : "0" + sec) + "." +
		(ms > 9 ? ms : ms);
};

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

function getClickedNodeDesc() {
	console.log('getClickedNodeDesc(): ' + clickedNodeName);
	sceneJSON.assets.forEach(function (element) {
		if (element.id == clickedNodeName) {
			clickedNodeDescr = element.descr;
		}
	});
}

function getClickedNodeOrientation() {
	console.log('getClickedNodeOrientation(): ' + clickedNodeName);
	var nodeName = { name: clickedNodeName };
	doPostRequest(apiEndPoint + "/nodeDerivedOrientation", nodeName, function (res) {
		clickedNodeOrientation= res.data.items[0].orientation;
		console.log('getClickedNodeDerivedOrientation(): res: ', clickedNodeOrientation);
	});
}

function getClickedNodePosition() {
	console.log('getClickedNodeDerivedPosition(): ' + clickedNodeName);
	var nodeName = { name: clickedNodeName};
	doPostRequest(apiEndPoint + "/nodeDerivedPosition", nodeName, function (res) {
		clickedNodePosition = res.data.items[0].position;
		console.log('getClickedNodeDerivedPosition(): res: ', clickedNodePosition);
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

function freeMe() {
	console.log('try to freeMe');
	doPostRequest(apiEndPoint + "/nodes/" + userNodeName + "/" + userID + '/owner', function (res) {
		var ownerID = res.data.items[0].ownerID;
		console.log('post setOwner() res: ', ownerID);
		doPostRequest(apiEndPoint + "/nodes/" + userNodeName + "/" + "root" + '/parent', function (res) {
			var parentNodeName = res.data.items[0].parentName;
			console.log('post setParent() zero: res: ', parentNodeName);
		});
	});
	var pos = { x: 0, y: 0, z: 0 };
	setNodePosition(userNodeName, pos);
	var ori = { w: 1, x: 0, y: 0, z: 0 };
	setNodeOrientation(userNodeName, ori);
}

function attachOtherUserNode2Me(otherUserNodeName) {
	var pos = { x: 0, y: 0, z: 0 };
	setNodePosition(otherUserNodeName, pos);
	var ori = { w: 1, x: 0, y: 0, z: 0 };
	setNodeOrientation(otherUserNodeName, ori);
	console.log('try to attachOtherUserNode2Me: ', otherUserNodeName);
	doGetRequest(apiEndPoint + "/nodes/" + otherUserNodeName + "/" + userNodeName + '/parent', function (res) {
		var currentParentNodeName = res.data.items[0].parentName;
		console.log('get parentNodeName(): currentParentNodeName: ', currentParentNodeName);
		if (currentParentNodeName == userNodeName) {
			doPostRequest(apiEndPoint + "/nodes/" + otherUserNodeName + "/" + "root" + '/parent', function (res) {
				var parentNodeName = res.data.items[0].parentName;
				console.log('post setParent() zero: res: ', parentNodeName);
				$('#users').toggle();
				var pos = otherUserNodeName.lastIndexOf("_");
				var otherUserID = otherUserNodeName.substring(pos + 1, otherUserNodeName.length);
				doPostRequest(apiEndPoint + "/nodes/" + otherUserNodeName + "/" + otherUserID + '/owner', function (res) {
					var ownerID = res.data.items[0].ownerID;
					console.log('post setOwner() res: ', ownerID);
				});
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

function fireHoverIn() {
	console.log('fireHoverIn()');
	var commandMSG = { command: 'fireHoverIn' };
	doPostRequest(apiEndPoint + "/overLayBrowserFireHoverIn", commandMSG, function (res) {
		console.log('fireHoverIn(): res: ', res);
	});
}

function fireHoverOut() {
	console.log('fireHoverOut(): ');
	var commandMSG = { command: 'fireHoverOut' };
	doPostRequest(apiEndPoint + "/overLayBrowserFireHoverOut", commandMSG, function (res) {
		console.log('fireHoverOut(): res: ', res);
	});
}

function setClickedElement(clickedElementName) {
	console.log('setClickedElement(): ', clickedElementName);
	var clickedElementNameMSG = { name: clickedElementName };
	doPostRequest(apiEndPoint + "/overLayBrowserSetClickedElement", clickedElementNameMSG, function (res) {
		console.log('setClickedElement(): res: ', res);
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
	console.log('doPostRequest(): ', data);
    $.post(apiEndPointUrl, data, function(res) {
        console.log('doPostRequest(): ', res);
        callback(res);
    }, "json");
}

function showPauseAndSkipButtons() {
	console.log('showPauseAndSkipButtons');
	$('#leftButtonsPauseSkip').show();
	$('#play').hide();
	updateOverlayBrowserLastMessageInterval = setInterval(getOverlayBrowserLastMessage, 50);
}

function hidePauseAndSkipButtons() {
	console.log('hidePauseAndSkipButtons');
	$('#leftButtonsPauseSkip').hide();
	$('#play').show();
	clearInterval(updateOverlayBrowserLastMessageInterval);
}

function showChat() {
    console.log('toogle chat');
	$('#chat').toggle();
}

function toogleFreeMe() {
	$('#freeMe').toggle();
}

function toogleFreeUsers() {
	$('#attachUsers').toggle();
	$('#freeUsers').toggle();
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
				//attachOtherUserNode2Me(element.name);
			});
			usersDiv.appendChild(newDiv);
		}
	});
}

function updateMap() {
	//console.log('update map');
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
		//console.log('relativePosition: ', relativePosition)
		ctx.beginPath();
		ctx.arc((canvas.width / 2) - relativePosition.x, (canvas.height / 2) - relativePosition.y, 5, 0, 2 * Math.PI);
		ctx.stroke();
		index++;
	});
}

function showMap() {
    console.log('toogle map');
	$('#map').toggle();
	if ($('#map').is(":hidden")) {
		clearInterval(updateMapInterval);
	}
	else {
		updateMapInterval = setInterval(updateMap, 40);
	}
}

function toogleBookmarks() {
	if ($('#bookmarks').is(":hidden")) {
		showPauseAndSkipButtons();
		showBookmarks();
	}
	else {
		hidePauseAndSkipButtons();
		$('#bookmarks').hide();
	}
}

function showBookmarks() {
	console.log('toogle bookmarks');
	$('#bookmarks').show();
	var bookmarksDiv = document.getElementById('bookmarks');
	animationJSON.bookmarks.forEach(function (element) {
		//console.log('bookmark: ' + element.name);
		var bookmarkDiv = document.getElementById(element.name);
		if (typeof (bookmarkDiv) != 'undefined' && bookmarkDiv != null) {
			bookmarkDiv.innerHTML = element.name + '@' + element.time / 1000 + ' sec(s)';
			console.log('already bookmark div: ' + element.name);
		}
		else {
			var newDiv = document.createElement('div');
			newDiv.id = element.name;
			newDiv.innerHTML = element.name + '@' + element.time / 1000 + ' sec(s)';
			newDiv.addEventListener('click', function () {
				setClickedElement(element.name + '@' + element.time);
				$('#leftButtonsPauseSkip').show();
				$('#play').hide();
			});
			bookmarksDiv.appendChild(newDiv);
			console.log('new bookmark div: ' + element.name);
		}
	});
}

function parseAnimationJSON() {
	var pos = roomName.search("vlft");
	var roomSTR = roomName.substring(pos + 4, roomName.length);
	var url = "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/local" + roomSTR + "/apeVLFTAnimationPlayerPlugin.json";
	console.log('parseAnimationJSON' + url);
	$.get(url, function (json) {
		animationJSON = json;
		console.log("JSON Data: " + JSON.stringify(json));
	});
}

function parseSceneJSON() {
	var pos = roomName.search("vlft");
	var roomSTR = roomName.substring(pos + 4, roomName.length);
	var url = "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/local" + roomSTR + "/apeVLFTSceneLoaderPlugin.json";
	console.log('parseSceneJSON' + url);
	$.get(url, function (json) {
		sceneJSON = json;
		console.log("JSON Data: " + JSON.stringify(json));
	});
}

function updateProperties() {
	//console.log('update Properties of the clicked node: ' + clickedNodeName);
	getClickedNodePosition();
	document.getElementById('selectedNodePosition').innerHTML = 'Position: (' + clickedNodePosition.x + ',' + clickedNodePosition.y + ',' + clickedNodePosition.z + ')';
	getClickedNodeOrientation();
	document.getElementById('selectedNodeOrientation').innerHTML = 'Orientation: (' + clickedNodeOrientation.w + ',' + clickedNodeOrientation.x + ',' + clickedNodeOrientation.y + ',' + clickedNodeOrientation.z + ')';
	getClickedNodeDesc();
	document.getElementById('selectedNodeDescription').innerHTML = 'Description: ' + clickedNodeDescr;
}

function hideStudentButtons() {
	console.log('hideStudentButtons');
	$('#freeMe').hide();
}

function hideTeacherButtons() {
	console.log('hideTeacherButtons');
	$('#attachUsers').hide();
	$('#otherusers').hide();
	$('#showUsers').hide();
	$('#logUsers').hide();
}

function hideMultiUserButtons() {
	console.log('hideMultiUserButtons');
	$('#attach').hide();
	$('#otherusers').hide();
	$('#messenger').hide();
	$('#chat').hide();
}

function toggleLogUsersStop() {
	$('#logUsers').toggle();
	$('#logUsersStop').toggle();
}

function toggleScreencastStop() {
	$('#screencast').toggle();
	$('#screencastStop').toggle();
}

function toggleInfoSection() {
	$('#infoSection').toggle();
}

$(document).ready(function () {
	getRoomName();
	getUserNodeNameAndID();
	getOtherUserNodeNames();
	getUserNodePosition();
	getOtherUserNodePositions();
	$('#map').toggle();
	$('#users').toggle();
	$('#bookmarks').toggle();
	$('#screencastStop').toggle();
	$('#freeUsers').toggle();
	$('#freeMe').toggle();
	$('#logUsersStop').toggle();
	hidePauseAndSkipButtons();
    var sock = new WebSocket("ws://localhost:40080/ws");
	sock.onopen = () => {
		console.log('open');
		getUserNodeNameAndID();
		var isStudent = userNodeName.indexOf("VLFT_Student");
		if (isStudent != -1) {
			hideTeacherButtons();
			updateMeAttachedInterval = setInterval(updateMeAttached, 40);
		}
		var isLocal = userNodeName.indexOf("VLFT_Local");
		if (isLocal != -1) {
			hideMultiUserButtons();
			hideStudentButtons();
			hideTeacherButtons();
		}
		var isTeacher = userNodeName.indexOf("VLFT_Teacher");
		if (isTeacher != -1) {
			hideStudentButtons();
		}
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
		
		if (eventObj.type == 11) { 
			$("#nodeName").val(eventObj.subjectName);
			clickedNodeName = eventObj.subjectName;
			console.log(' show bounding box - select: ', clickedNodeName);
			document.getElementById('selectedNodeNameTitle').innerHTML = clickedNodeName;
			updatePropertiesInterval = setInterval(updateProperties, 40);
        }
    }
    $("button").click(function () {
    	console.log('click on: ', this.id);
    	$(this).css('background-color', 'green');
    	setClickedElement(this.id);
    });
    $("button").hover(
    function () {
		if (this.id != "")
			$(this).css("background-color", convertHex('#FFFFFF', 0.7));
    }, function () {
		if (this.id != "")
			$(this).css("background-color", convertHex('#FFFFFF', 0.7));
    });
    $("div").hover(
    function () {
    	if (this.id != "")
    		console.log('hover in: ' + this.id);
    	    fireHoverIn();
    }, function () {
    	if (this.id != "")
    		console.log('hover out: ' + this.id);
    		fireHoverOut();
    });
    $("iframe").hover(
   function () {
   	if (this.id != "")
   		console.log('hover in: ' + this.id);
   	    fireHoverIn();
   }, function () {
   	if (this.id != "")
   		console.log('hover out: ' + this.id);
   		fireHoverOut();
   });
});
