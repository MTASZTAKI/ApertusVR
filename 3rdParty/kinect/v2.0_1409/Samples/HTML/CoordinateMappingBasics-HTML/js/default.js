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
    var coordinateMapperImageProcessor = null;

    // references to html objects
    var canvas = null;
    var context = null;

    // active Kinect sensor
    var sensor = null;

    // Reader for depth/color/body index frames
    var multiFrameSourceReader = null;

    // containers for frame data
    var colorFrameBuffer = null;
    var depthFrameData = null;
    var bodyIndexFrameData = null;

    var bufferManager = null;

    // Canvas image helper uses webgl to output a bitmap to the canvas.
    // Webgl is used because it is significantly faster for drawing and every millisecond
    // counts in this sample
    var canvasImageHelper = new CanvasImageHelper();

    // Handles the depth/color/body index frame data arriving from the sensor
    function reader_MultiSourceFrameArrived(args) {
        // get multiframereference
        var multiFrameReference = args.frameReference.acquireFrame();

        if (multiFrameReference != null) {
            // get various frames
            var colorFrame = multiFrameReference.colorFrameReference.acquireFrame();
            var depthFrame = multiFrameReference.depthFrameReference.acquireFrame();
            var bodyIndexFrame = multiFrameReference.bodyIndexFrameReference.acquireFrame();

            var gotData = false;

            // we got all frames
            if ((depthFrame != null) && (colorFrame != null) && (bodyIndexFrame != null)) {
                // copy color, depth, and bodyIndex data to containers
                colorFrame.copyConvertedFrameDataToBuffer(colorFrameBuffer, kinect.ColorImageFormat.rgba);

                depthFrame.copyFrameDataToArray(depthFrameData);

                bodyIndexFrame.copyFrameDataToArray(bodyIndexFrameData);

                gotData = true;
            }

            // Clean up acquired frames
            if (colorFrame != null) {
                colorFrame.close();
            }

            if (depthFrame != null) {
                depthFrame.close();
            }

            if (bodyIndexFrame != null) {
                bodyIndexFrame.close();
            }
            
            if (multiFrameReference != null) {
                multiFrameReference.close();
            }

            if (gotData) {
                var outputBuffer = bufferManager.acquireBuffer();

                // Don't process frame if we don't have buffers available to process it
                if (outputBuffer != null) {

                    // Yield control temporarily so that any pending updates to the UI can be drawn
                    // before we do more compute-intensive things that will lock up the UI thread
                    setImmediate(function (displayPixelBuffer) {

                        // call native WinRT component to process data and copy to canvas ImageData pixel array
                        if (coordinateMapperImageProcessor.processMultiFrameBufferData(depthFrameData, colorFrameBuffer, bodyIndexFrameData, displayPixelBuffer.buffer)) {

                            // put image bits back onto canvas
                            displayPixelBuffer.transferToArrayBufferAsync()
                                .done(function (arrayBuffer) {
                                    canvasImageHelper.processImageData(arrayBuffer, canvas.width, canvas.height);
                                    displayPixelBuffer.close();
                                });
                        }
                    }, outputBuffer);
                }
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

                coordinateMapperImageProcessor = KinectImageProcessor.CoordinateMapperHelper();

                // get the kinectSensor object
                sensor = kinect.KinectSensor.getDefault();

                // add handler for sensor availability
                sensor.addEventListener("isavailablechanged", sensor_IsAvailableChanged);

                // open the reader for various frames
                multiFrameSourceReader = sensor.openMultiSourceFrameReader(kinect.FrameSourceTypes.depth | kinect.FrameSourceTypes.color | kinect.FrameSourceTypes.bodyIndex);
               
                // wire handler for frame arrival
                multiFrameSourceReader.addEventListener("multisourceframearrived", reader_MultiSourceFrameArrived);

                // get frameDescription for color and depth frame sources
                var depthFrameDescription = sensor.depthFrameSource.frameDescription;
                var colorFrameDescription = sensor.colorFrameSource.createFrameDescription(kinect.ColorImageFormat.rgba);

                // create depth, color, and bodyIndex containers
                depthFrameData = new Uint16Array(depthFrameDescription.lengthInPixels);
                colorFrameBuffer = new streams.Buffer(colorFrameDescription.width * colorFrameDescription.height * colorFrameDescription.bytesPerPixel);
                bodyIndexFrameData = new Uint8Array(depthFrameDescription.width * depthFrameDescription.height);

                // keeps track of buffers that we will use for our output image
                bufferManager = new ImageBufferManager(colorFrameDescription.lengthInPixels * colorFrameDescription.bytesPerPixel, 2);

                // get the ImageData object from canvas and bind it to our canvas image helper
                canvas = document.getElementById("mainCanvas");
                canvas.width = colorFrameDescription.width;
                canvas.height = colorFrameDescription.height;
                canvasImageHelper.bindToCanvas(canvas);

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
        if (multiFrameSourceReader != null) {
            multiFrameSourceReader.close();
        }

        if (sensor != null) {
            sensor.close();
        }
    }

    app.start();
})();

// Make a copy of the canvas to avoid overwriting the original.
function cloneCanvas(oldCanvas) {
    // Create a new canvas.
    var newCanvas = document.createElement('canvas');
    var context = newCanvas.getContext('2d');

    // Copy the dimensions of the old canvas.
    newCanvas.width = oldCanvas.width;
    newCanvas.height = oldCanvas.height;

    // Copy the contents of the old canvas to the new one.
    context.drawImage(oldCanvas, 0, 0);

    // Return the new canvas.
    return newCanvas;
}

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

    var canvas = document.getElementById("mainCanvas");
    var clonedCanvas = cloneCanvas(canvas);                             // Make a copy of the canvas.
    var backgroundImage = document.getElementById("background");        // Get the background image.
    var clonedCanvasContext = clonedCanvas.getContext("2d");
    clonedCanvasContext.globalCompositeOperation = "destination-over";  // Draw under the existing picture.
    clonedCanvasContext.drawImage(backgroundImage, 0, 0);               // Composite the background image under the clone.
    var blob = clonedCanvas.msToBlob();                                 // Get the bitmap of the cloned canvas with the new background.
    clonedCanvas = null;                                                // Encourage garbage collection of the clone.

    var input = null;
    var fileStream = null;
    var finalizeSave = function () {
        if (input != null) { input.close(); input = null; }
        if (fileStream != null) { fileStream.close(); fileStream = null; }
        isSavingFile = false;
    };
    var date = new Date();
    var time = date.getHours() + '-' + date.getMinutes() + '-' + date.getSeconds();
    var fileName = "KinectScreenshot-CoordinateMapping-" + time + ".png";

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