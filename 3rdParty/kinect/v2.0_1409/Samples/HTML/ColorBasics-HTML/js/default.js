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

    // references to canvas
    var colorCanvas = null;

    // active Kinect sensor
    var sensor = null;

    // reader for color frames
    var colorFrameReader = null;

    // reference to canvas ImageData
    var colorBufferManager = null;
    var canvasImageHelper = new CanvasImageHelper();

    // Handles the color frame data arriving from the sensor
    function reader_ColorFrameArrived(args) {
        var colorFrame = args.frameReference.acquireFrame();
        if (colorFrame == null) {
            return;
        }

        var colorBuffer = colorBufferManager.acquireBuffer();
        if (colorBuffer == null) {
            // Don't process frame if we don't have buffers available to process it
            return;
        }
        
        // copy color frame data
        colorFrame.copyConvertedFrameDataToBuffer(colorBuffer.buffer, kinect.ColorImageFormat.rgba);
        colorFrame.close();
        colorBuffer.transferToArrayBufferAsync()
            .done(function (arrayBuffer) {
                canvasImageHelper.processImageData(arrayBuffer, colorCanvas.width, colorCanvas.height);

                colorBuffer.close();
            });
    }

    // Handler for sensor availability changes
    function sensor_IsAvailableChanged(args) {
        if (sensor.isAvailable) {
            document.getElementById("statustext").innerHTML = "Running";
        }
        else {
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

                // open the reader for color frames
                colorFrameReader = sensor.colorFrameSource.openReader();

                // wire handler for frame arrival
                colorFrameReader.addEventListener("framearrived", reader_ColorFrameArrived);

                // create the colorFrameDescription for RGBA format
                var colorFrameDescription = sensor.colorFrameSource.createFrameDescription(kinect.ColorImageFormat.rgba);

                // get the canvas and ImageData
                colorCanvas = document.getElementById("mainCanvas");
                colorCanvas.width = colorFrameDescription.width;
                colorCanvas.height = colorFrameDescription.height;
                colorBufferManager = new ImageBufferManager(colorFrameDescription.lengthInPixels * colorFrameDescription.bytesPerPixel);
                canvasImageHelper.bindToCanvas(colorCanvas);

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
        if (colorFrameReader != null) {
            colorFrameReader.close();
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

    var fileName = "KinectScreenshot-Color-" + time + ".png";

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