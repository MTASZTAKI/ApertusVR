var apiEndPoint = 'http://localhost:3000/api/v1/';
var apiEndPointNode = apiEndPoint + 'nodes/';
var apiEndPointFileGeometries = apiEndPoint + 'filegeometries/';
var apiEndPointCalcTrans = apiEndPoint + 'calcTransform/';
var nodeName = 'ur10Gripper';

function genNodeItem(name) {
	console.log('genNodeItem(): ', name);
	var r = $('<a class="dropdown-item" href="#" onClick="getNodeLocalTrans(\'' + name + '\')">' + name + '</a>');
	$("#nodesDropDownList").append(r);
	$('#nodesDropDown').dropdown();
}

function getNodeLocalTrans(nodeName) {
	console.log('getNodeLocalTrans(): ', nodeName);
}

function enableEditor(val) {
    //$("body *").attr("disabled", !val);
}

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

function getNodePosition(nodeName) {
    console.log('getNodePosition(): ', nodeName);
    doGetRequest(apiEndPointNode + nodeName + '/position', function(res){
        var pos = res.data.items[0].position;
        console.log('getNodePosition(): res: ', pos);
        $('#posX').val(pos.x);
        $('#posY').val(pos.y);
        $('#posZ').val(pos.z);
    });
}

function setNodePosition(nodeName) {
    console.log('setNodePosition(): ', nodeName);
    var pos = {
        x: $('#posX').val(),
        y: $('#posY').val(),
        z: $('#posZ').val()
    };
    doPostRequest(apiEndPointNode + nodeName + '/position', pos, function(res){
        console.log('setNodePosition(): res: ', res);
        getTransformationMatrix(nodeName);
    });
}

function getNodeOrientation(nodeName) {
    console.log('getNodeOrientation(): ', nodeName);
    doGetRequest(apiEndPointNode + nodeName + '/orientation', function(res){
        var ori = res.data.items[0].orientation;
        console.log('getNodeOrientation(): res: ', ori);

        $('#quatW').val(ori.w);
        $("#quatWRange").slider("value", ori.w);

        $('#quatX').val(ori.x);
        $("#quatXRange").slider("value", ori.x);

        $('#quatY').val(ori.y);
        $("#quatYRange").slider("value", ori.y);

        $('#quatZ').val(ori.z);
        $("#quatZRange").slider("value", ori.z);
    });
}

function setNodeOrientation(nodeName) {
    console.log('setNodeOrientation(): ', nodeName);
    var ori = {
        w: $('#quatW').val(),
        x: $('#quatX').val(),
        y: $('#quatY').val(),
        z: $('#quatZ').val()
    };
    doPostRequest(apiEndPointNode + nodeName + '/orientation', ori, function(res){
        console.log('setNodeOrientation(): res: ', res);
        getNodeEuler(nodeName);
        getTransformationMatrix(nodeName);
    });
}

function getNodeEuler(nodeName) {
    console.log('getNodeEuler(): ', nodeName);
    doGetRequest(apiEndPointNode + nodeName + '/euler', function(res){
        var euler = res.data.items[0].euler;
        console.log('getNodeEuler(): res: ', euler);

        $('#eulerY').val(euler.y);
        $("#eulerYRange").slider("value", euler.y);

        $('#eulerP').val(euler.p);
        $("#eulerPRange").slider("value", euler.p);

        $('#eulerR').val(euler.r);
        $("#eulerRRange").slider("value", euler.r);
    });
}

function setNodeEuler(nodeName) {
    console.log('setNodeEuler(): ', nodeName);
    var euler = {
        y: $('#eulerY').val(),
        p: $('#eulerP').val(),
        r: $('#eulerR').val()
    };
    doPostRequest(apiEndPointNode + nodeName + '/euler', euler, function(res){
        console.log('setNodeEuler(): res: ', res);
        //getNodeOrientation(nodeName);
        getTransformationMatrix(nodeName);
    });
}

function getTransformationMatrix(nodeName) {
	console.log('getTransformationMatrix(): ', nodeName);
	doGetRequest(apiEndPointNode + nodeName + '/transformationmatrix', function (res) {

		var matrix = res.data.items[0].transformationmatrix;
		var invRotMatrix = res.data.items[0].invRotTransMatrix;

        console.log('getTransformationMatrix(): res: ', matrix);
        $('#transMatrix').val(matrix);
		//$('#invRotTransMatrix').val(invRotMatrix);
    });
}

function calcTransformMatrix() {
	if ($('#transNodeName').val().length > 0) {
		doGetRequest(apiEndPointCalcTrans + nodeName + '/' + $('#transNodeName').val(), function (res2) {
			var matrix2 = res2.data.items[0].transformation.invRotTransMatrix;
			console.log(matrix2);
			$('#invRotTransMatrix').val(matrix2);
		});
	}
}

function getNodeScale(nodeName) {
    console.log('getNodeScale(): ', nodeName);
    doGetRequest(apiEndPointNode + nodeName + '/scale', function(res){
        var scale = res.data.items[0].scale;
        console.log('getNodeScale(): res: ', scale);
        $('#scaleX').val(scale.x);
        $('#scaleY').val(scale.y);
        $('#scaleZ').val(scale.z);
    });
}

function setNodeScale(nodeName) {
    console.log('setNodeScale(): ', nodeName);
    var scale = {
        x: $('#scaleX').val(),
        y: $('#scaleY').val(),
        z: $('#scaleZ').val()
    };
    doPostRequest(apiEndPointNode + nodeName + '/scale', scale, function(res){
        console.log('setNodeScale(): res: ', res);
        getTransformationMatrix(nodeName);
    });
}

function updateProperties() {
    $('#nodeName').val(nodeName);
    getNodePosition(nodeName);
    getNodeScale(nodeName);

    getNodeOrientation(nodeName);
    getNodeEuler(nodeName);

    getTransformationMatrix(nodeName);
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

function showToast(text, timeOutMs) {
    $('#snackbar').html(text);
    $('#snackbar').addClass('show');
    setTimeout(function(){
        $('#snackbar').removeClass('show');
    }, timeOutMs);
}

function copyToClipboard(elementId) {
    var element = document.getElementById(elementId);
    element.select();
    document.execCommand('Copy');
    showToast('Text Copied: ' + element.value, 3000);
}

function pasteFromClipboard(elementId) {
    var element = document.getElementById(elementId);
    element.focus();
    element.select();
    document.execCommand('Paste');
}

function resetPos() {
    console.log('resetPos');

    $('#posX').val(0);
    $('#posY').val(0);
    $('#posZ').val(0);

    setNodePosition(nodeName);
}

function resetQuat() {
    console.log('resetQuat');

    $('#quatW').val(1);
    $("#quatWRange").slider("value", 1);

    $('#quatX').val(0);
    $("#quatXRange").slider("value", 0);

    $('#quatY').val(0);
    $("#quatYRange").slider("value", 0);

    $('#quatZ').val(0);
    $("#quatZRange").slider("value", 0);

    setNodeOrientation(nodeName);
}

function resetEuler() {
    console.log('resetEuler');

    $('#eulerY').val(0);
    $("#eulerYRange").slider("value", 0);

    $('#eulerP').val(0);
    $("#eulerPRange").slider("value", 0);

    $('#eulerR').val(0);
    $("#eulerRRange").slider("value", 0);

    setNodeEuler(nodeName);
}

function resetScale() {
    console.log('resetScale');

    $('#scaleX').val(1);
    $('#scaleY').val(1);
    $('#scaleZ').val(1);

    setNodeScale(nodeName);
}

function onFilePathChange(files) {
    if (files.length > 0) {
        var selectedFile = files[0];
        console.log('selectedFile:', selectedFile);

        // create a FormData object which will be sent as the data payload in the
        // AJAX request
        var formData = new FormData();

        // loop through all the selected files and add them to the formData object
        for (var i = 0; i < files.length; i++) {
            var file = files[i];

            // add the files to formData object for the data payload
            formData.append('uploads[]', file, file.name);
        }

        $.ajax({
            url: apiEndPointFileGeometries,
            type: 'POST',
            data: formData,
            processData: false,
            contentType: false,
            success: function(data){
                console.log('upload successful!\n' + data);
            }
        });
    }
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

        if (eventObj.type == 9) { // show bounding box - select
            enableEditor(true);

            $("#nodeName").val(eventObj.subjectName);
            nodeName = eventObj.subjectName;

            getNodePosition(nodeName);
            getNodeOrientation(nodeName);
            getNodeEuler(nodeName);
            getNodeScale(nodeName);
            getTransformationMatrix(nodeName);
        }

        if (eventObj.type == 3) { // position
            getNodePosition(nodeName);
        }
        else if (eventObj.type == 4) { // orientation
            getNodeOrientation(nodeName);
            getNodeEuler(nodeName);
        }
        else if (eventObj.type == 5) { // scale
            getNodeScale(nodeName);
        }
        getTransformationMatrix(nodeName);
    }

    $("#nodeName").change(function(){
        nodeName = $(this).val();
    });

    $('[data-toggle="tooltip"]').tooltip();

    $(".input-pos").bind('keyup change', function(e){
        console.log('pos bind: ', $(this).val());
        setNodePosition(nodeName);
    });

    $(".input-scale").bind('keyup change', function(e){
        console.log('scale bind: ', $(this).val());
        setNodeScale(nodeName);
    });

    $(".input-range-quat").slider({
        min: -1,
        max: 1,
        value: 0,
        step: 0.00000001,
        slide: function( event, ui ) {
            var focused = document.activeElement;
            if (focused.classList.contains('ui-slider-handle')) {
                $("#" + $(this).data("input-handler")).prop('value', ui.value);
                setNodeOrientation(nodeName);
            }
        }
    });

    $(".input-range-quat-var").bind('keyup change', function(e){
        console.log('quat bind: ', $(this).val());
        var focused = document.activeElement;
        if (focused.classList.contains('input-range-quat-var')) {
            $("#" + $(this).data("input-range-quat")).slider("value", $(this).val());
            setNodeOrientation(nodeName);
        }
    });

    $(".input-range-euler").slider({
        min: -180,
        max: 180,
        value: 0,
        step: 0.00000001,
        slide: function( event, ui ) {
            var focused = document.activeElement;
            if (focused.classList.contains('ui-slider-handle')) {
                $("#" + $(this).data("input-handler")).prop('value', ui.value);
                setNodeEuler(nodeName);
            }
        }
    });

    $(".input-range-euler-var").bind('keyup change', function(e){
        console.log('euler bind: ', $(this).val());
        var focused = document.activeElement;
        if (focused.classList.contains('input-range-euler-var')) {
            $("#" + $(this).data("input-range-euler")).slider("value", $(this).val());
            setNodeEuler(nodeName);
        }
    });

    $('#openModelButton').click(function(){
        $('#filePath').click();
    });

    enableEditor(false);

    updateProperties();

    getNodesNames();
});
