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
    var face = Microsoft.Kinect.Face;

    // thickness of face bounding box and face points
    var DRAW_FACE_SHAPE_THICKNESS = 8;

    // radius of face points
    var DRAW_FACE_POINT_RADIUS = 3;

    // text layout offset from the head joint
    var DRAW_TEXT_LAYOUT_OFFSET_X = -0.1;

    /// Text layout offset in Y axis
    var DRAW_TEXT_LAYOUT_OFFSET_Y = -0.175;

    // text height
    var DRAW_TEXT_HEIGHT = 30;

    // vertical text spacing between each text row
    var DRAW_TEXT_VERTICAL_SPACING = 2;

    // face rotation display angle increment in degrees
    var ROTATIONINCREMENT_IN_DEGREES = 5.0;

    // text to indicate that there are no bodies/faces tracked in the FOV
    var NO_FACE_TRACKED_TEXT = "No bodies or faces are tracked ...";

    // text layout in X for the no face tracked message
    var NO_FACE_TRACKED_TEXT_LAYOUT_X = 20;

    // text layout in Y for the no face tracked message
    var NO_FACE_TRACKED_TEXT_LAYOUT_Y = 40;

    // text color for the no face tracked message
    var NO_FACE_TRACKED_TEXT_COLOR = "white";

    // references to canvas
    var faceCanvas = null;
    var faceContext = null;

    // active Kinect sensor
    var sensor = null;

    // coordinate mapper to map one type of point to another
    var coordinateMapper = null;

    // reader for body frames
    var bodyFrameReader = null;

    // sources of face frames
    var faceFrameSources = new Array();

    // readers for face frames
    var faceFrameReaders = new Array();

    // storage for face frame results
    var faceFrameResults = new Array();

    // number of bodies tracked
    var bodyCount = 0;

    // array to store bodies
    var bodies = null;

    // defines a different color for each face
    var faceColors = null;

    app.onactivated = function (args) {
        if (args.detail.kind === activation.ActivationKind.launch) {
            if (args.detail.previousExecutionState !== activation.ApplicationExecutionState.terminated) {
                // get the kinectSensor object
                sensor = kinect.KinectSensor.getDefault();

                // get the coordinate mapper
                coordinateMapper = sensor.coordinateMapper;

                // get the color frame description
                var colorFrameDescription = sensor.colorFrameSource.frameDescription;

                // get canvas objects
                faceCanvas = document.getElementById("mainCanvas");
                faceCanvas.width = colorFrameDescription.width;
                faceCanvas.height = colorFrameDescription.height;
                faceContext = faceCanvas.getContext("2d");

                // open the reader for body frames
                bodyFrameReader = sensor.bodyFrameSource.openReader();

                // wire handler for body frame arrival
                bodyFrameReader.addEventListener("framearrived", reader_BodyFrameArrived);

                // set the maximum number of bodies that would be tracked by Kinect
                bodyCount = sensor.bodyFrameSource.bodyCount;

                // allocate storage to store body objects
                bodies = new Array(sensor.bodyFrameSource.bodyCount);

                // specify the required face frame results
                var faceFrameFeatures =
                    face.FaceFrameFeatures.boundingBoxInColorSpace |
                    face.FaceFrameFeatures.pointsInColorSpace |
                    face.FaceFrameFeatures.rotationOrientation |
                    face.FaceFrameFeatures.faceEngagement |
                    face.FaceFrameFeatures.glasses |
                    face.FaceFrameFeatures.happy |
                    face.FaceFrameFeatures.leftEyeClosed |
                    face.FaceFrameFeatures.rightEyeClosed |
                    face.FaceFrameFeatures.lookingAway |
                    face.FaceFrameFeatures.mouthMoved;

                // create a face frame source + reader to track each face in the FOV
                var initialTrackingId = 0;
                for (var i = 0; i < bodyCount; i++) {
                    // create the face frame source with the required face frame features and an initial tracking Id of 0
                    faceFrameSources[i] = face.FaceFrameSource(sensor, initialTrackingId, faceFrameFeatures);

                    // open the corresponding reader
                    faceFrameReaders[i] = faceFrameSources[i].openReader();

                    // wire handler for face frame arrival
                    faceFrameReaders[i].addEventListener("framearrived", createReaderFrameHandler(i));

                    // initialize each face frame result
                    faceFrameResults[i] = null;
                }

                // populate face result colors - one for each face index
                faceColors = [
                    "white",
                    "orange",
                    "green",
                    "red",
                    "lightBlue",
                    "yellow"
                ];

                // add handler for sensor availability
                sensor.addEventListener("isavailablechanged", sensor_IsAvailableChanged);

                // open the sensor
                sensor.open();
            }

            args.setPromise(WinJS.UI.processAll());
        }
    };

    // function to create the face frame arrived event handler with the face index
    function createReaderFrameHandler(index) {
        return function (args) {
            reader_FaceFrameArrived(index, args);
        };
    }

    // handles the face frame data arriving from the sensor
    function reader_FaceFrameArrived(index, args) {
        var faceFrame = args.frameReference.acquireFrame();

        if (faceFrame != null) {
            // check if this face frame has valid face frame results
            if (validateFaceBoxAndPoints(faceFrame.faceFrameResult)) {
                // store this face frame result to draw later
                faceFrameResults[index] = faceFrame.faceFrameResult;
            }
            else {
                // indicates that the latest face frame result from this reader is invalid
                faceFrameResults[index] = null;
            }

            faceFrame.close();
        }
    }

    // handles the body frame data arriving from the sensor
    function reader_BodyFrameArrived(args) {
        var bodyFrame = args.frameReference.acquireFrame();

        if (bodyFrame != null) {
            // update body data
            bodyFrame.getAndRefreshBodyData(bodies);

            // clear canvas before drawing each frame
            faceContext.clearRect(0, 0, faceCanvas.width, faceCanvas.height);

            var drawFaceResult = false;

            // iterate through each face source
            for (var i = 0; i < bodyCount; ++i) {
                // check if a valid face is tracked in this face source
                if (faceFrameSources[i].isTrackingIdValid) {
                    // check if we have valid face frame results
                    if (faceFrameResults[i] != null) {
                        // draw face frame results
                        drawFaceFrameResults(i, faceFrameResults[i]);

                        if (!drawFaceResult) {
                            drawFaceResult = true;
                        }
                    }
                }
                else {
                    // check if the corresponding body is tracked 
                    if (bodies[i].isTracked) {
                        // update the face frame source to track this body
                        faceFrameSources[i].trackingId = bodies[i].trackingId;
                    }
                }
            }

            if (!drawFaceResult) {
                // if no faces were drawn then this indicates one of the following:
                // a body was not tracked 
                // a body was tracked but the corresponding face was not tracked
                // a body and the corresponding face was tracked though the face box or the face points were not valid
                drawNoFacesTrackedText();
            }

            bodyFrame.close();
        }
    }

    // draws face frame results
    var drawFaceFrameResults = function (faceIndex, faceFrameResult) {
        // choose the face color based on the face index
        var faceColor = faceColors[0];
        if (faceIndex < bodyCount) {
            faceColor = faceColors[faceIndex];
        }

        // draw the face bounding box
        var faceBox = faceFrameResult.faceBoundingBoxInColorSpace;
        if (faceBox != null) {
            drawFaceBox(faceBox, faceColor);
        }

        var facePoints = faceFrameResult.facePointsInColorSpace;
        if (facePoints != null) {
            // draw each face point
            for (var i = 0; i < facePoints.size; i++) {
                drawFacePoint(facePoints.lookup(i), faceColor);
            }
        }

        var faceText = [];

        var faceProp = faceFrameResult.faceProperties;
        if (faceProp != null) {
            for (var i = 0; i < faceProp.size; i++) {
                // extract each face property information and store it is faceText
                faceText[faceText.length] = lookupFaceProperty(i, faceProp.lookup(i));
            }
        }

        var faceRot = faceFrameResult.faceRotationQuaternion;
        if (faceRot != null) {
            // extract face rotation in degrees as Euler angles and store them in the faceText array
            appendRotationText(faceRot, faceText);
        }

        // render the face property and face rotation information
        var faceTextLayout = { x: 0.0, y: 0.0 };
        if (getFaceTextPositionInColorSpace(faceIndex, faceTextLayout)) {
            drawText(faceText, faceColor, faceTextLayout.x, faceTextLayout.y);
        }
    }

    // computes the face result text position by adding an offset to the corresponding 
    // body's head joint in camera space and then by projecting it to screen space
    var getFaceTextPositionInColorSpace = function (faceIndex, faceTextLayout) {
        var isLayoutValid = false;

        var body = bodies[faceIndex];
        if (body.isTracked) {
            var i = kinect.JointType.head;
            var headJoint = body.joints.lookup(i).position;

            var textPoint =
            {
                x: headJoint.x + DRAW_TEXT_LAYOUT_OFFSET_X,
                y: headJoint.y + DRAW_TEXT_LAYOUT_OFFSET_Y,
                z: headJoint.z
            };

            var textPointInColor = coordinateMapper.mapCameraPointToColorSpace(textPoint);

            faceTextLayout.x = textPointInColor.x;
            faceTextLayout.y = textPointInColor.y;
            isLayoutValid = true;
        }

        return isLayoutValid;
    }

    // validates face bounding box and face points to be within screen space
    var validateFaceBoxAndPoints = function (faceResult) {
        var isFaceValid = faceResult != null;

        if (isFaceValid) {
            var faceBox = faceResult.faceBoundingBoxInColorSpace;
            if (faceBox != null) {
                var width = faceBox.right - faceBox.left;
                var height = faceBox.bottom - faceBox.top;

                // check if we have a valid rectangle within the bounds of the screen space
                isFaceValid = width > 0 &&
                              height > 0 &&
                              faceBox.right <= faceCanvas.width &&
                              faceBox.bottom <= faceCanvas.height;

                if (isFaceValid) {
                    var facePoints = faceResult.facePointsInColorSpace;
                    if (facePoints != null) {
                        for (var i = 0; i < facePoints.size; i++) {
                            var pointF = facePoints.lookup(i);
                            // check if we have a valid face point within the bounds of the screen space
                            var isFacePointValid = pointF.x > 0.0 &&
                                                   pointF.y > 0.0 &&
                                                   pointF.x < faceCanvas.width &&
                                                   pointF.y < faceCanvas.height;
                            if (!isFacePointValid) {
                                isFaceValid = false;
                                break;
                            }
                        }
                    }
                }
            }
        }

        return isFaceValid;
    }

    // lookup the text corresponding to a face property
    var lookupFaceProperty = function (prop, result) {
        var st = "";

        switch (prop) {
            case face.FaceProperty.happy:
                st = "Happy :";
                break;
            case face.FaceProperty.engaged:
                st = "Engaged :";
                break;
            case face.FaceProperty.leftEyeClosed:
                st = "LeftEyeClosed :";
                break;
            case face.FaceProperty.rightEyeClosed:
                st = "RightEyeClosed :";
                break;
            case face.FaceProperty.lookingAway:
                st = "LookingAway :";
                break;
            case face.FaceProperty.mouthMoved:
                st = "MouthMoved :";
                break;
            case face.FaceProperty.mouthOpen:
                st = "MouthOpen :";
                break;
            case face.FaceProperty.wearingGlasses:
                st = "WearingGlasses :";
                break;
            default:
                break;
        }

        switch (result) {
            case kinect.DetectionResult.unknown:
                st = st.concat(" UnKnown");
                break;
            case kinect.DetectionResult.yes:
                st = st.concat(" Yes");
                break;

            // consider a "maybe" as a "no" to restrict 
            // the detection result refresh rate
            case kinect.DetectionResult.no:
            case kinect.DetectionResult.maybe:
                st = st.concat(" No");
                break;
            default:
                break;
        }

        return st;
    }

    // draws a face bounding box
    var drawFaceBox = function (faceBox, faceColor) {
        faceContext.beginPath();
        faceContext.strokeStyle = faceColor;
        faceContext.lineWidth = DRAW_FACE_SHAPE_THICKNESS;
        faceContext.fillStyle = faceColor;
        faceContext.rect(faceBox.left, faceBox.top, faceBox.right - faceBox.left, faceBox.bottom - faceBox.top);
        faceContext.stroke();
        faceContext.closePath();
    }

    // draws a face point
    var drawFacePoint = function (facePoint, faceColor) {
        faceContext.beginPath();
        faceContext.strokeStyle = faceColor;
        faceContext.lineWidth = DRAW_FACE_SHAPE_THICKNESS;
        faceContext.fillStyle = faceColor;
        faceContext.arc(facePoint.x, facePoint.y, DRAW_FACE_POINT_RADIUS, 0, Math.PI * 2, true);
        faceContext.fill();
        faceContext.stroke();
        faceContext.closePath();
    }

    // converts rotation quaternion to Euler angles 
    // and then maps them to a specified range of values to control the refresh rate
    var appendRotationText = function (faceQuaternion, faceText) {
        var x = faceQuaternion.x;
        var y = faceQuaternion.y;
        var z = faceQuaternion.z;
        var w = faceQuaternion.w;

        // convert face rotation quaternion to Euler angles in degrees
        var pitch = Math.atan2(2 * (y * z + w * x), w * w - x * x - y * y + z * z) / Math.PI * 180.0;
        var yaw = Math.asin(2 * (w * y - x * z)) / Math.PI * 180.0;
        var roll = Math.atan2(2 * (x * y + w * z), w * w + x * x - y * y - z * z) / Math.PI * 180.0;

        // clamp the values to a multiple of the specified increment to control the refresh rate
        var increment = ROTATIONINCREMENT_IN_DEGREES;
        pitch = Math.floor((pitch + ((increment / 2.0) * (pitch > 0 ? 1.0 : -1.0))) / increment) * increment;
        yaw = Math.floor((yaw + ((increment / 2.0) * (yaw > 0 ? 1.0 : -1.0))) / increment) * increment;
        roll = Math.floor((roll + ((increment / 2.0) * (roll > 0 ? 1.0 : -1.0))) / increment) * increment;

        faceText[faceText.length] = "FaceYaw : " + parseFloat(yaw).toFixed(0);
        faceText[faceText.length] = "FacePitch : " + parseFloat(pitch).toFixed(0);
        faceText[faceText.length] = "FaceRoll :" + parseFloat(roll).toFixed(0);
    }

    // draws text contained in the faceText array with each string on a new line
    var drawText = function (faceText, faceColor, textLayoutX, textLayoutY) {
        faceContext.beginPath();
        faceContext.strokeStyle = faceColor;
        faceContext.font = DRAW_TEXT_HEIGHT + "px Georgia";
        faceContext.fillStyle = faceColor;

        for (var row = 0; row < faceText.length; ++row) {
            var textYPosition = textLayoutY + row * DRAW_TEXT_HEIGHT + DRAW_TEXT_VERTICAL_SPACING;
            faceContext.fillText(faceText[row], textLayoutX, textYPosition);
            faceContext.fill();
            faceContext.stroke();
        }

        faceContext.closePath();
    }

    // draws text to indicate that there are no bodies/faces tracked in the FOV
    var drawNoFacesTrackedText = function () {
        faceContext.beginPath();
        faceContext.strokeStyle = NO_FACE_TRACKED_TEXT_COLOR;
        faceContext.font = DRAW_TEXT_HEIGHT + "px Georgia";
        faceContext.fillStyle = NO_FACE_TRACKED_TEXT_COLOR;
        faceContext.fillText(NO_FACE_TRACKED_TEXT, NO_FACE_TRACKED_TEXT_LAYOUT_X, NO_FACE_TRACKED_TEXT_LAYOUT_Y);
        faceContext.fill();
        faceContext.stroke();
        faceContext.closePath();
    }

    // handler for sensor availability changes
    function sensor_IsAvailableChanged(args) {
        if (sensor.isAvailable) {
            document.getElementById("statustext").innerHTML = "Running";
        }
        else {
            document.getElementById("statustext").innerHTML = "Kinect not available!";
        }
    }

    app.onunload = function (args) {
        for (var i = 0; i < bodyCount; i++) {
            if (faceFrameReaders[i] != null) {
                faceFrameReaders[i].close();
            }

            if (faceFrameSources[i] != null) {
                faceFrameSources[i].close();
            }
        }

        if (bodyFrameReader != null) {
            bodyFrameReader.close();
        }

        if (sensor != null) {
            sensor.close();
        }
    }
    app.start();
})();
