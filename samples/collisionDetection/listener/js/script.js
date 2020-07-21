$(document).ready(function () {
    var sock = new WebSocket("ws://localhost:40080/ws");
	sock.onopen = () => {
		console.log('open');
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
        if (eventObj.type == 6410/*RIGIDBODY_COLLISION*/) {
            var messagesSelect = document.getElementById('messagesSelect');
            var newOption = document.createElement('option');
            newOption.innerHTML = eventObj.subjectName;
            messagesSelect.appendChild(newOption);
        }
    }
});
