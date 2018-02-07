var apiEndPoint = 'http://localhost:3000/api/v1/';
var apiEndPointNode = apiEndPoint + 'nodes/';
var nodeName = 'robotRootNode';

function getNodePosition(nodeName) {
    console.log('getting Node position: ', nodeName);
    doGetRequest(apiEndPointNode + nodeName + '/position', function(res){
        var pos = res.data.items[0].position;
        console.log(pos);
        $('#posX').val(pos.x.trim());
        $('#posY').val(pos.y.trim());
        $('#posZ').val(pos.z.trim());
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
        getTransformationMatrix(nodeName);
    });
}

function getNodeOrientation(nodeName) {
    console.log('getting Node orientation: ', nodeName);
    doGetRequest(apiEndPointNode + nodeName + '/orientation', function(res){
        var ori = res.data.items[0].orientation;
        console.log(ori);

        $('#quatW').val(ori.w.trim());
        $("#quatWRange").slider("value", ori.w.trim());

        $('#quatX').val(ori.x.trim());
        $("#quatXRange").slider("value", ori.x.trim());

        $('#quatY').val(ori.y.trim());
        $("#quatYRange").slider("value", ori.y.trim());

        $('#quatZ').val(ori.z.trim());
        $("#quatZRange").slider("value", ori.z.trim());
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
        getNodeEuler(nodeName);
        getTransformationMatrix(nodeName);
    });
}

function getNodeEuler(nodeName) {
    console.log('getting Node euler: ', nodeName);
    doGetRequest(apiEndPointNode + nodeName + '/euler', function(res){
        var euler = res.data.items[0].euler;
        console.log(euler);

        $('#eulerY').val(euler.y);
        $("#eulerYRange").slider("value", euler.y);

        $('#eulerP').val(euler.p);
        $("#eulerPRange").slider("value", euler.p);

        $('#eulerR').val(euler.r);
        $("#eulerRRange").slider("value", euler.r);
    });
}

function setNodeEuler(nodeName) {
    console.log('setting Node euler: ', nodeName);
    var euler = {
        y: $('#eulerY').val(),
        p: $('#eulerP').val(),
        r: $('#eulerR').val()
    };
    doPostRequest(apiEndPointNode + nodeName + '/euler', euler, function(res){
        console.log(res);
        getNodeOrientation(nodeName);
        getTransformationMatrix(nodeName);
    });
}

function getTransformationMatrix(nodeName) {
    console.log('getting Node transformation matrix: ', nodeName);
    doGetRequest(apiEndPointNode + nodeName + '/transformationmatrix', function(res){
        var matrix = res.data.items[0].transformationmatrix;
        console.log(matrix);
        $('#transMatrix').val(matrix);
    });
}

function getNodeScale(nodeName) {
    console.log('getting Node scale: ', nodeName);
    doGetRequest(apiEndPointNode + nodeName + '/scale', function(res){
        var scale = res.data.items[0].scale;
        console.log(scale);
        $('#scaleX').val(scale.x);
        $('#scaleY').val(scale.y);
        $('#scaleZ').val(scale.z);
    });
}

function setNodeScale(nodeName) {
    console.log('setting Node scale: ', nodeName);
    var scale = {
        x: $('#scaleX').val(),
        y: $('#scaleY').val(),
        z: $('#scaleZ').val()
    };
    doPostRequest(apiEndPointNode + nodeName + '/scale', scale, function(res){
        console.log(res);
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
        console.log(res);
        callback(res);
    });
}

function doPostRequest(apiEndPointUrl, data, callback) {
    $.post(apiEndPointUrl, data, function(res) {
        console.log(res);
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

    $('#scaleX').val(0);
    $('#scaleY').val(0);
    $('#scaleZ').val(0);

    setNodeScale(nodeName);
}

$(document).ready(function(){

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

    updateProperties();
});
