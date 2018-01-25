$.support.cors = true;

var apiEndPoint = 'http://localhost:3000/api/v1/';
var apiEndPointNode = apiEndPoint + 'nodes/';
var nodeName = 'coordinateSystemArrowXConeNode';

// -------------

function getNodePosition(nodeName) {
    console.log('getting Node position: ', nodeName);
    doGetRequest(apiEndPointNode + nodeName + '/position', function(res){
        var pos = res.data.items[0].position;
        console.log(pos);
        $('#posX').val(pos.x);
        $('#posY').val(pos.y);
        $('#posZ').val(pos.z);
    });
}

function setNodePosition(nodeName) {
    console.log('setting Node position: ', nodeName);
    var pos = {
        x: $('#posX').val(),
        y: $('#posY').val(),
        z: $('#posZ').val()
    };
    doPostRequest(apiEndPointNode + nodeName + '/position', pos, function(res){
        console.log(res);
    });
}

function getNodeOrientation(nodeName) {
    console.log('getting Node orientation: ', nodeName);
    doGetRequest(apiEndPointNode + nodeName + '/orientation', function(res){
        var ori = res.data.items[0].orientation;
        console.log(ori);
        $('#quatW').val(ori.w);
        $('#quatX').val(ori.x);
        $('#quatY').val(ori.y);
        $('#quatZ').val(ori.z);
    });
}

function setNodeOrientation(nodeName) {
    console.log('setting Node orientation: ', nodeName);
    var ori = {
        w: $('#quatW').val(),
        x: $('#quatX').val(),
        y: $('#quatY').val(),
        z: $('#quatZ').val()
    };
    doPostRequest(apiEndPointNode + nodeName + '/orientation', ori, function(res){
        console.log(res);
    });
}

// -------------

function doGetRequest(apiEndPointUrl, callback) {
    $.get(apiEndPointUrl, function(res) {
        console.log(res);
        callback(res);
    });
}

function doPostRequest(apiEndPointUrl, data, callback) {
    $.post(apiEndPointUrl, data, function( res ) {
        console.log(res);
        callback(res);
    }, "json");
}

// -------------

$(document).ready(function(){
    $('#nodeName').val(nodeName);
    getNodePosition(nodeName);
    getNodeOrientation(nodeName);

    $("#nodeName").change(function() {
        nodeName = $( this ).val();
    });

    $("#posSubmit").click(function(){
        setNodePosition(nodeName);
    });
    $("#quatSubmit").click(function(){
        setNodeOrientation(nodeName);
    });
});
