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
var clickedNodeState;
var log;
var roomName;
var configFolderPath;
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

function getConfigFolderPath() {
	console.log('getConfigFolderPath()');
	doGetRequest(apiEndPoint + '/getFolderPath', function (res) {
		configFolderPath = res.data.items[0].configFolderPath;
		console.log('getConfigFolderPath(): res: ', configFolderPath);
	});
}

function getOverlayBrowserLastMessage() {
	console.log('getOverlayBrowserLastMessage()');
	doGetRequest(apiEndPoint + '/overLayBrowserGetLastMessage', function (res) {
		lastMessage = res.data.items[0].lastMessage;
		console.log('getOverlayBrowserLastMessage(): res: ', lastMessage);
		updateAnimationTime(lastMessage);
	});
}

function getClickedNodeState(sec) {
	console.log('getClickedNodeState(): ' + sec);
	animationJSON.nodes.forEach(function (node) {
		if (node.name == clickedNodeName) {
			node.actions.forEach(function (action) {
				if (action.event.type == "state" && action.trigger.type == "timestamp") {
					if (action.trigger.data == sec) {
						clickedNodeState = action.event.descr;
					}
				}
			});
		}
	});
}

function getLog() {
	console.log('getLog(): ');
	var logsDiv = document.getElementById('Log');
	animationJSON.nodes.forEach(function (node) {
		node.actions.forEach(function (action) {
			if (action.event.type == "link") {
				var logDiv = document.getElementById(action.event.data);
				if (typeof (logDiv) != 'undefined' && logDiv != null) {
					logDiv.innerHTML = action.event.descr + " was saved to " + action.event.data;
				}
				else {
					var newDiv = document.createElement('div');
					newDiv.id = action.event.data;
					newDiv.innerHTML = action.event.descr + " was saved to " + action.event.data;
					logsDiv.appendChild(newDiv);
				}
			}
		});
	});
}

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

function findDescr(parentName) {
	for (let asset of sceneJSON.assets) {
		if (asset.id == parentName) {
			if (asset.descr) {
				clickedNodeDescr = asset.descr;
				break;
			}
			else {
				if (asset.parentObject) {
					findDescr(asset.parentObject);
				}
			}
		}
	};
}

function getClickedNodeDesc() {
	console.log('getClickedNodeDesc(): ' + clickedNodeName);
	for (let asset of sceneJSON.assets) {
		if (asset.id == clickedNodeName) {
			clickedNodeDescr = clickedNodeName;
			if (asset.descr) {
				clickedNodeDescr = asset.descr;
				break;
			}
			else {
				if (asset.parentObject) {
					findDescr(asset.parentObject);
				}
			}
		}
	};
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

function startRoom() {
	var roomName = "vlftTestCase07";
	console.log('startRoom: ' + roomName);
	var commandMSG = { command: 'startRoom/:' + roomName};
	doPostRequest('http://193.224.59.230:8888', commandMSG, function (res) {
		console.log('startRoom(): res: ', res);
	});
}

function stopRoom() {
	var roomName = "vlftTestCase07";
	console.log('stopRoom: ' + roomName);
	var commandMSG = { command: 'stopRoom/:' + roomName };
	doPostRequest('http://193.224.59.230:8888', commandMSG, function (res) {
		console.log('stopRoom(): res: ', res);
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
	$('#logs').hide();
	updateOverlayBrowserLastMessageInterval = setInterval(getOverlayBrowserLastMessage, 50);
}

function hidePauseAndSkipButtons() {
	console.log('hidePauseAndSkipButtons');
	$('#leftButtonsPauseSkip').hide();
	$('#play').show();
	$('#logs').show();
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

function toggleLog() {
	$('#Log').toggle();
}

function sendConnectParams() {
	var selectedRoom;
	var selectedUserType;
	var selectedUserName;
	if (document.getElementById('radioLocal').checked)
		selectedUserType = "_Local";
	if (document.getElementById('radioTeacher').checked)
		selectedUserType = "_Teacher";
	if (document.getElementById('radioStudent').checked)
		selectedUserType = "_Student";
	var selectRoom = document.getElementById("selectRoom");
	selectedRoom = selectRoom.options[selectRoom.selectedIndex].innerHTML;
	roomName = selectedRoom;
	selectedUserName = document.getElementById("usr").value;
	setClickedElement('connect' + ';userType:' + selectedUserType + ';roomName:' + selectedRoom + ';userName:' + selectedUserName);
	showDesiredMenu(selectedUserType);
	getConfigFolderPath();
	parseAnimationJSON();
	parseSceneJSON();
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
	//var pos = roomName.search("vlft");
	//var roomSTR = roomName.substring(pos + 4, roomName.length);
	var url = "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/rooms/" + roomName + "/apeVLFTAnimationPlayerPlugin.json";
	console.log('parseAnimationJSON' + url);
	$.get(url, function (json) {
		animationJSON = json;
		console.log("JSON Data: " + JSON.stringify(json));
	});
	/*var fileToSave = new Blob([JSON.stringify(animationJSON)], {
		type: 'application/json',
		name: 'apeVLFTAnimationPlayerPlugin.json'
	});
	console.log('saveAs: ' + configFolderPath + 'apeVLFTAnimationPlayerPlugin.json');
	saveAs(fileToSave, configFolderPath + 'apeVLFTAnimationPlayerPlugin.json');*/
}

function parseSceneJSON() {
	//var pos = roomName.search("vlft");
	//var roomSTR = roomName.substring(pos + 4, roomName.length);
	var url = "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/rooms/" + roomName + "/apeVLFTSceneLoaderPlugin.json";
	console.log('parseSceneJSON' + url);
	$.get(url, function (json) {
		sceneJSON = json;
		console.log("JSON Data: " + JSON.stringify(json));
	});
	/*var fileToSave = new Blob([JSON.stringify(sceneJSON)], {
		type: 'application/json',
		name: 'apeVLFTSceneLoaderPlugin.json'
	});
	console.log('saveAs: ' + configFolderPath + 'apeVLFTSceneLoaderPlugin.json');
	saveAs(fileToSave, configFolderPath + 'apeVLFTSceneLoaderPlugin.json');*/
}

function updateProperties() {
	//console.log('update Properties of the clicked node: ' + clickedNodeName);
	getClickedNodePosition();
	document.getElementById('selectedNodePosition').innerHTML = 'Position: (' + clickedNodePosition.x + ',' + clickedNodePosition.y + ',' + clickedNodePosition.z + ')';
	getClickedNodeOrientation();
	document.getElementById('selectedNodeOrientation').innerHTML = 'Orientation: (' + clickedNodeOrientation.w + ',' + clickedNodeOrientation.x + ',' + clickedNodeOrientation.y + ',' + clickedNodeOrientation.z + ')';
	doGetRequest(apiEndPoint + '/overLayBrowserGetLastMessage', function (res) {
		lastMessage = res.data.items[0].lastMessage;
		console.log('getOverlayBrowserLastMessage(): res: ', lastMessage);
		var sec = Math.floor((lastMessage / 1000) % 60);
		getClickedNodeState(sec);
		document.getElementById('selectedNodeState').innerHTML = 'State: ' + clickedNodeState;
	});
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

function prepareUploadRoom() {
	var fileCatcher = document.getElementById('file-catcher');
	var fileInput = document.getElementById('file-input');
	var fileListDisplay = document.getElementById('file-list-display');
	var fileList = [];
	var renderFileList, sendFile;
	fileCatcher.addEventListener('submit', function (evnt) {
		evnt.preventDefault();
		fileList.forEach(function (file) {
			sendFile(file);
		});
	});
	fileInput.addEventListener('change', function (evnt) {
		fileList = [];
		for (var i = 0; i < fileInput.files.length; i++) {
			fileList.push(fileInput.files[i]);
		}
		renderFileList();
	});
	renderFileList = function () {
		fileListDisplay.innerHTML = '';
		fileList.forEach(function (file, index) {
			var fileDisplayEl = document.createElement('p');
			fileDisplayEl.innerHTML = (index + 1) + ': ' + file.name;
			fileListDisplay.appendChild(fileDisplayEl);
		});
	};
	sendFile = function (file) {
		var selectedRoomName = document.getElementById("uploadRoomFolder").value;
		var url = "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/rooms/";//+ selectedRoomName;
		console.log("sendFile::UploadRoom: " + url);
		var formData = new FormData();
		var request = new XMLHttpRequest();
		formData.set('file', file);
		request.open("POST", url);
		request.send(formData);
	};
}

function isRoomRunning(roomName, roomIDPostFix, selectName) {
	console.log("isRoomRunning: " + roomName);
	doGetRequest(apiEndPoint + '/roomRunning/' + roomName, function (res) {
		isRunning = res.data.items[0].isRunning;
		console.log('isRoomRunning(): res: ', isRunning);
		var select = document.getElementById(selectName);
		var option = document.getElementById(roomName + roomIDPostFix);
		if (selectName == "roomsToStart") {
			if (isRunning) {
				select.removeChild(option);
			}
		}
		else {
			if (!isRunning) {
				select.removeChild(option);
			}
		}
	});
}

function listRoomsToStart() {
	var uploadedRooms = [];
	var url = "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/rooms/";
	console.log('listRoomsToStart: ' + url);
	var roomsToStart = document.getElementById('roomsToStart');
	$.get(url, function (data) {
		//console.log("data: " + data);
		var posFolders = data.indexOf("[DIR]");
		var foldersSTR = data.substring(posFolders, data.length);
		var posFolderStart = 0;
		while (posFolderEnd != -1) {
			posFolderStart = foldersSTR.indexOf('/">');
			if (posFolderStart == -1) {
				break;
			}
			var posFolderEnd = foldersSTR.indexOf('/</a>');
			var folderSTR = foldersSTR.substring(posFolderStart + 3, posFolderEnd);
			foldersSTR = foldersSTR.substring(posFolderEnd + 5, foldersSTR.length);
			var option = document.getElementById(folderSTR + "_4Start");
			if (typeof (option) != 'undefined' && option != null) {
				option.innerHTML = folderSTR;
				uploadedRooms.push(folderSTR);
			}
			else {
				var newOption = document.createElement('option');
				newOption.id = folderSTR + "_4Start";
				newOption.innerHTML = folderSTR;
				roomsToStart.appendChild(newOption);
				uploadedRooms.push(folderSTR);
			}
			//console.log("posFolderStart: " + posFolderStart + " posFolderEnd: " + posFolderEnd);
			console.log("folderSTR: " + folderSTR);
			//console.log("foldersSTR: " + foldersSTR);
		};
		uploadedRooms.forEach(function (uploadeRoom) {
			isRoomRunning(uploadeRoom, "_4Start", 'roomsToStart');
		});
	});
}

function listUploadedRooms() {
	var url = "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/rooms/";
	console.log('listUploadedRooms: ' + url);
	var uploadedRoomsSelect = document.getElementById('uploadedRooms');
	$.get(url, function (data) {
		//console.log("data: " + data);
		var posFolders = data.indexOf("[DIR]");
		var foldersSTR = data.substring(posFolders, data.length);
		var posFolderStart = 0;
		while (posFolderEnd != -1) {
			posFolderStart = foldersSTR.indexOf('/">');
			if (posFolderStart == -1) {
				break;
			}
			var posFolderEnd = foldersSTR.indexOf('/</a>');
			var folderSTR = foldersSTR.substring(posFolderStart + 3, posFolderEnd);
			foldersSTR = foldersSTR.substring(posFolderEnd + 5, foldersSTR.length);
			var option = document.getElementById(folderSTR + "_uploaded");
			if (typeof (option) != 'undefined' && option != null) {
				option.innerHTML = folderSTR;
			}
			else {
				var newOption = document.createElement('option');
				newOption.id = folderSTR + "_uploaded";
				newOption.innerHTML = folderSTR;
				uploadedRoomsSelect.appendChild(newOption);
			}
			//console.log("posFolderStart: " + posFolderStart + " posFolderEnd: " + posFolderEnd);
			console.log("folderSTR: " + folderSTR);
			//console.log("foldersSTR: " + foldersSTR);
		};
	});
}

function listRunningRooms() {
	var uploadedRooms = [];
	var url = "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/rooms/";
	console.log('listRunningRooms: ' + url);
	var runningRooms = document.getElementById('runningRooms');
	$.get(url, function (data) {
		//console.log("data: " + data);
		var posFolders = data.indexOf("[DIR]");
		var foldersSTR = data.substring(posFolders, data.length);
		var posFolderStart = 0;
		while (posFolderEnd != -1) {
			posFolderStart = foldersSTR.indexOf('/">');
			if (posFolderStart == -1) {
				break;
			}
			var posFolderEnd = foldersSTR.indexOf('/</a>');
			var folderSTR = foldersSTR.substring(posFolderStart + 3, posFolderEnd);
			foldersSTR = foldersSTR.substring(posFolderEnd + 5, foldersSTR.length);
			var option = document.getElementById(folderSTR + "_running");
			if (typeof (option) != 'undefined' && option != null) {
				option.innerHTML = folderSTR;
				uploadedRooms.push(folderSTR);
			}
			else {
				var newOption = document.createElement('option');
				newOption.id = folderSTR + "_running";
				newOption.innerHTML = folderSTR;
				runningRooms.appendChild(newOption);
				uploadedRooms.push(folderSTR);
			}
			//console.log("posFolderStart: " + posFolderStart + " posFolderEnd: " + posFolderEnd);
			console.log("folderSTR: " + folderSTR);
			//console.log("foldersSTR: " + foldersSTR);
		};
		uploadedRooms.forEach(function (uploadeRoom) {
			isRoomRunning(uploadeRoom, "_running", 'runningRooms');
		});
	});
}

function refreshAvailableRooms() {
	var uploadedRooms = [];
	var url = "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/rooms/";
	console.log('refreshAvailableRooms: ' + url);
	var selectRoom = document.getElementById('selectRoom');
	$.get(url, function (data) {
		//console.log("data: " + data);
		var posFolders = data.indexOf("[DIR]");
		var foldersSTR = data.substring(posFolders, data.length);
		var posFolderStart = 0;
		while (posFolderEnd != -1) {
			posFolderStart = foldersSTR.indexOf('/">');
			if (posFolderStart == -1) {
				break;
			}
			var posFolderEnd = foldersSTR.indexOf('/</a>');
			var folderSTR = foldersSTR.substring(posFolderStart + 3, posFolderEnd);
			foldersSTR = foldersSTR.substring(posFolderEnd + 5, foldersSTR.length);
			var option = document.getElementById(folderSTR);
			if (typeof (option) != 'undefined' && option != null) {
				option.innerHTML = folderSTR;
				uploadedRooms.push(folderSTR);
			}
			else {
				var newOption = document.createElement('option');
				newOption.id = folderSTR;
				newOption.innerHTML = folderSTR;
				selectRoom.appendChild(newOption);
				uploadedRooms.push(folderSTR);
			}
			//console.log("posFolderStart: " + posFolderStart + " posFolderEnd: " + posFolderEnd);
			console.log("folderSTR: " + folderSTR);
			//console.log("foldersSTR: " + foldersSTR);
		}
		if (document.getElementById('radioTeacher').checked || document.getElementById('radioStudent').checked) {
			uploadedRooms.forEach(function (uploadeRoom) {
				isRoomRunning(uploadeRoom, "", 'selectRoom');
			});
		}
	});
}

function showDesiredMenu(userName) {
	var isStudent = userName.indexOf("_Student");
	if (isStudent != -1) {
		$('#lobbyMenu').children().hide();
		$('#adminMenuLeft').children().hide();
		$('#adminMenuRight').children().hide();
		$('#leftMenu').children().show();
		$('#rightMenu').children().show();
		hideTeacherButtons();
		$('#bookmarks').hide();
		$('#Log').hide();
		updateMeAttachedInterval = setInterval(updateMeAttached, 40);
	}
	var isLocal = userName.indexOf("_Local");
	if (isLocal != -1) {
		$('#lobbyMenu').children().hide();
		$('#adminMenuLeft').children().hide();
		$('#adminMenuRight').children().hide();
		$('#leftMenu').children().show();
		$('#rightMenu').children().show();
		hideMultiUserButtons();
		hideStudentButtons();
		hideTeacherButtons();
		$('#bookmarks').hide();
		$('#Log').hide();
	}
	var isTeacher = userName.indexOf("_Teacher");
	if (isTeacher != -1) {
		$('#lobbyMenu').children().hide();
		$('#adminMenuLeft').children().hide();
		$('#adminMenuRight').children().hide();
		$('#leftMenu').children().show();
		$('#rightMenu').children().show();
		hideStudentButtons();
		$('#bookmarks').hide();
		$('#Log').hide();
	}
	var isLobby = userName.indexOf("_Lobby");
	if (isLobby != -1) {
		$('#lobbyMenu').show();
		$('#adminMenuLeft').children().hide();
		$('#adminMenuRight').children().hide();
		$('#leftMenu').children().hide();
		$('#rightMenu').children().hide();
	}
	var isAdmin = userName.indexOf("_Admin");
	if (isAdmin != -1) {
		$('#adminMenuLeft').children().show();
		$('#adminMenuRight').children().show();
		$('#lobbyMenu').children().hide();
		$('#leftMenu').children().hide();
		$('#rightMenu').children().hide();
		$('#chat').hide();
	}
}

$(document).ready(function () {
	getUserNodeNameAndID();
	getOtherUserNodeNames();
	getUserNodePosition();
	getOtherUserNodePositions();
	$('#map').toggle();
	$('#users').toggle();
	$('#bookmarks').toggle();
	$('#Log').toggle();
	$('#screencastStop').toggle();
	$('#freeUsers').toggle();
	$('#freeMe').toggle();
	$('#logUsersStop').toggle();
	hidePauseAndSkipButtons();
	showDesiredMenu("_Lobby");
    var sock = new WebSocket("ws://localhost:40080/ws");
	sock.onopen = () => {
		console.log('open');
		getUserNodeNameAndID();
		//getLog();
		showDesiredMenu(userNodeName);
		var isAdmin = userNodeName.indexOf("_Admin");
		if (isAdmin != -1) {
			prepareUploadRoom();
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
			updatePropertiesInterval = setInterval(updateProperties, 40);
			$("#nodeName").val(eventObj.subjectName);
			clickedNodeName = eventObj.subjectName;
			console.log(' show bounding box - select: ', clickedNodeName);
			document.getElementById('selectedNodeNameTitle').innerHTML = clickedNodeName;
			getClickedNodeDesc();
			document.getElementById('selectedNodeDescription').innerHTML = 'Description: ' + clickedNodeDescr;
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
    $("button").hover(
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
