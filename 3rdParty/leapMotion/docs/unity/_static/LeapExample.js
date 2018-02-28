window.leap = new Leap.Controller();
window.leap.connect();
 
function LeapExample(exampleID, initCallback, frameCallback) {
    var self = this;
    this.exampleID = exampleID;
    this.initCallback = initCallback;
    this.frameCallback = frameCallback;
    
    this.leapConnected = false;
    this.initialized = false;
    this.controller = new Leap.Controller();
    this.init = function(){
        self.exampleElement = document.getElementById(self.exampleID);
        if(self.initCallback) self.initialized = self.initCallback();
        self.leapConnected = true;
        self.exampleElement.style.display = "block";
    }
    this.controller.on('deviceConnected', function() {
        if(!self.initialized) self.init();
        self.leapConnected = true;
        self.exampleElement.style.display = "block";
    });

    this.controller.on('deviceDisconnected', function() {
        self.leapConnected = false; 
        self.exampleElement.style.display = "none";
    });
    
    this.controller.on('frame', function() {
        if(!self.initialized) self.init();
        if(self.frameCallback) self.frameCallback(self.controller.frame(0));
    });

    this.controller.connect();
}
