//------------------------------------------------------------------------------
// <copyright file="default.js" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

(function () {
    "use strict";

    WinJS.Binding.optimizeBindingReferences = true;

    var app = WinJS.Application;
    var activation = Windows.ApplicationModel.Activation;
    var streams = Windows.Storage.Streams;
    var kinect = WindowsPreview.Kinect;

    // C++ WinRT component
    var depthImageProcessor = KinectImageProcessor.DepthHelper;

    // reference to canvas
    var depthCanvas = null;
    var depthContext = null

    // active Kinect sensor
    var sensor = null;

    // reader for depth frames
    var depthFrameReader = null;

    // reference to canvas ImageData
    var depthPixels = null;

    // storage for depth frame data
    var depthDataArray = null;

    // Handles the depth frame data arriving from the sensor
    function reader_DepthFrameArrived(args) {
        // get depth frame
        var depthFrame = args.frameReference.acquireFrame();

        if (depthFrame != null) {
            // copy depth frame data
            depthFrame.copyFrameDataToArray(depthDataArray);

            depthFrame.close();

            // call native WinRT component to copy 16bit depth data to canvas ImageData pixel array
            if (depthImageProcessor.createImagePixelDataFromArray(depthDataArray, depthPixels.data)) {
                // put imageData back onto canvas
                depthContext.clearRect(0, 0, depthCanvas.width, depthCanvas.height);
                depthContext.putImageData(depthPixels, 0, 0);
            }
        }
    }

    // handler for sensor availability changes
    function sensor_IsAvailableChanged(args) {
        if (sensor.isAvailable) {
            document.getElementById("statustext").innerHTML = "Running";
        } else {
            document.getElementById("statustext").innerHTML = "Kinect not available!";
        }
    }

    app.onactivated = function (args) {
        if (args.detail.kind === activation.ActivationKind.launch) {
            if (args.detail.previousExecutionState !== activation.ApplicationExecutionState.terminated) {

                // get the kinectSensor object
                sensor = kinect.KinectSensor.getDefault();

                // add handler for sensor availability
                sensor.addEventListener("isavailablechanged", sensor_IsAvailableChanged);

                // open the reader for depth frames
                depthFrameReader = sensor.depthFrameSource.openReader();

                // wire handler for frame arrival
                depthFrameReader.addEventListener("framearrived", reader_DepthFrameArrived);

                // get depthFrameDescription
                var depthFrameDescription = sensor.depthFrameSource.frameDescription;

                // get the canvas and ImageData
                depthCanvas = document.getElementById("mainCanvas");
                depthCanvas.width = depthFrameDescription.width;
                depthCanvas.height = depthFrameDescription.height;
                depthContext = depthCanvas.getContext("2d");
                depthPixels = depthContext.getImageData(0, 0, depthCanvas.width, depthCanvas.height);

                // allocate space for depth frame data
                depthDataArray = new Uint16Array(depthFrameDescription.lengthInPixels);

                // open the sensor
                sensor.open();

            } else {
                // TODO: This application has been reactivated from suspension.
                // Restore application state here.
            }
            args.setPromise(WinJS.UI.processAll());
        }
    };

    app.oncheckpoint = function (args) {
        // TODO: This application is about to be suspended. Save any state
        // that needs to persist across suspensions here. You might use the
        // WinJS.Application.sessionState object, which is automatically
        // saved and restored across suspension. If you need to complete an
        // asynchronous operation before your application is suspended, call
        // args.setPromise().
    };

    // Clean up
    app.onunload = function (args) {
        if (depthFrameReader != null) {
            depthFrameReader.close();
        }

        if (sensor != null) {
            sensor.close();
        }
    }

    app.start();
})();

// This Boolean guards against overlapping attempts to write to the same screenshot file.
var isSavingFile = false;

// Save a screenshot of the canvas to the Pictures Library folder.
function takeScreenshot() {
    // Take only one screenshot at a time.
    if (isSavingFile == true) {
        document.getElementById("statustext").innerHTML = "Screenshot save already in progress.  Please try again later.";
        return;
    } else {
        isSavingFile = true;
    }

    // Get the bitmap of the canvas.
    var blob = document.getElementById("mainCanvas").msToBlob();

    var input = null;
    var fileStream = null;
    var finalizeSave = function () {
        if (input != null) { input.close(); input = null; }
        if (fileStream != null) { fileStream.close(); fileStream = null; }
        isSavingFile = false;
    };
    var date = new Date();
    var time = date.getHours() + '-' + date.getMinutes() + '-' + date.getSeconds();
    var fileName = "KinectScreenshot-Depth-" + time + ".png";

    // Write the screenshot to the given file name.
    Windows.Storage.KnownFolders.picturesLibrary.createFileAsync(fileName,
              Windows.Storage.CreationCollisionOption.replaceExisting)
        .then(function (file) {
            return file.openAsync(Windows.Storage.FileAccessMode.readWrite);
        })
        .then(function (stream) {
            fileStream = stream;
            var output = stream.getOutputStreamAt(0);
            input = blob.msDetachStream();
            return Windows.Storage.Streams.RandomAccessStream.copyAndCloseAsync(input, output);
        })
        .done(
            function () {
                finalizeSave();
                document.getElementById("statustext").innerHTML = "Saved screenshot to Pictures Library as " + fileName;
            },
            function (err) {
                finalizeSave();
                document.getElementById("statustext").innerHTML = "Screenshot failed to save to Pictures Library!" + fileName;
            });
}