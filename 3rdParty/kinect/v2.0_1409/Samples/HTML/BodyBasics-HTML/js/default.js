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
    var bodyImageProcessor = KinectImageProcessor.BodyHelper;

    // references to canvas
    var bodyCanvas = null;
    var bodyContext = null;

    // active Kinect sensor
    var sensor = null;

    // reader for body frames
    var bodyFrameReader = null;

    // array of all bodies
    var bodies = null;

    // array of all bones in a body
    // bone defined by two joints
    var bones = null;

    // defines a different color for each body
    var bodyColors = null;

    // total number of joints = 25
    var jointCount = null;

    // total number of bones = 24
    var boneCount = null;

    // handstate circle size
    var HANDSIZE = 20;

    // tracked bone line thickness
    var TRACKEDBONETHICKNESS = 4;

    // inferred bone line thickness
    var INFERREDBONETHICKNESS = 1;

    // thickness of joints
    var JOINTTHICKNESS = 3;

    // thickness of clipped edges
    var CLIPBOUNDSTHICKNESS = 5;

    // closed hand state color
    var HANDCLOSEDCOLOR = "red";

    // open hand state color
    var HANDOPENCOLOR = "green";

    // lasso hand state color
    var HANDLASSOCOLOR = "blue";

    // tracked joint color
    var TRACKEDJOINTCOLOR = "green";

    // inferred joint color
    var INFERREDJOINTCOLOR = "yellow";

    // Handles the body frame data arriving from the sensor
    function reader_BodyFrameArrived(args) {
        // get body frame
        var bodyFrame = args.frameReference.acquireFrame();
        var dataReceived = false;

        if (bodyFrame != null) {
            // got a body, update body data
            bodyFrame.getAndRefreshBodyData(bodies);
            dataReceived = true;
            bodyFrame.close();
        }

        if (dataReceived) {
            // clear canvas before drawing each frame
            bodyContext.clearRect(0, 0, bodyCanvas.width, bodyCanvas.height);

            // iterate through each body
            for (var bodyIndex = 0; bodyIndex < bodies.length; ++bodyIndex) {                
                var body = bodies[bodyIndex];

                // look for tracked bodies
                if (body.isTracked) {
                    // get joints collection
                    var joints = body.joints;
                    // allocate space for storing joint locations
                    var jointPoints = createJointPoints();
                    
                    // call native component to map all joint locations to depth space
                    if (bodyImageProcessor.processJointLocations(joints, jointPoints)) {

                        // draw the body
                        drawBody(joints, jointPoints, bodyColors[bodyIndex]);

                        // draw handstate circles
                        updateHandState(body.handLeftState, jointPoints[kinect.JointType.handLeft]);
                        updateHandState(body.handRightState, jointPoints[kinect.JointType.handRight]);

                        // draw clipped edges if any
                        drawClippedEdges(body);
                    }
                }                    
            }
        }
    }

    // Draw a body
    var drawBody = function (joints, jointPoints, bodyColor) {
        // draw all bones
        for (var boneIndex = 0; boneIndex < boneCount; ++boneIndex) {

            var boneStart = bones[boneIndex].jointStart;
            var boneEnd = bones[boneIndex].jointEnd;

            var joint0 = joints.lookup(boneStart);
            var joint1 = joints.lookup(boneEnd);

            // don't do anything if either joint is not tracked
            if ((joint0.trackingState == kinect.TrackingState.notTracked) ||
                (joint1.trackingState == kinect.TrackingState.notTracked)) {
                return;
            }

            // all bone lines are inferred thickness unless both joints are tracked
            var boneThickness = INFERREDBONETHICKNESS;
            if ((joint0.trackingState == kinect.TrackingState.tracked) &&
                (joint1.trackingState == kinect.TrackingState.tracked)) {
                boneThickness = TRACKEDBONETHICKNESS;
            }

            drawBone(jointPoints[boneStart], jointPoints[boneEnd], boneThickness, bodyColor);
        }

        // draw all joints
        var jointColor = null;
        for (var jointIndex = 0; jointIndex < jointCount; ++jointIndex) {
            var trackingState = joints.lookup(jointIndex).trackingState;

            // only draw if joint is tracked or inferred
            if (trackingState == kinect.TrackingState.tracked) {
                jointColor = TRACKEDJOINTCOLOR;
            }
            else if (trackingState == kinect.TrackingState.inferred) {
                jointColor = INFERREDJOINTCOLOR;
            }

            if (jointColor != null) {
                drawJoint(jointPoints[jointIndex], jointColor);
            }
        }
    }

    // Draw a joint circle on canvas
    var drawJoint = function (joint, jointColor) {
        bodyContext.beginPath();
        bodyContext.fillStyle = jointColor;
        bodyContext.arc(joint.x, joint.y, JOINTTHICKNESS, 0, Math.PI * 2, true);
        bodyContext.fill();
        bodyContext.closePath();
    }

    // Draw a bone line on canvas
    var drawBone = function (startPoint, endPoint, boneThickness, boneColor) {
        bodyContext.beginPath();
        bodyContext.strokeStyle = boneColor;
        bodyContext.lineWidth = boneThickness;
        bodyContext.moveTo(startPoint.x, startPoint.y);
        bodyContext.lineTo(endPoint.x, endPoint.y);
        bodyContext.stroke();
        bodyContext.closePath();
    }

    // Determine hand state
    var updateHandState = function (handState, jointPoint) {
        switch (handState) {
            case kinect.HandState.closed:
                drawHand(jointPoint, HANDCLOSEDCOLOR);
                break;

            case kinect.HandState.open:
                drawHand(jointPoint, HANDOPENCOLOR);
                break;

            case kinect.HandState.lasso:
                drawHand(jointPoint, HANDLASSOCOLOR);
                break;                
        }
    }

    var drawHand = function (jointPoint, handColor) {
        // draw semi transparent hand cicles
        bodyContext.globalAlpha = 0.75;
        bodyContext.beginPath();
        bodyContext.fillStyle = handColor;
        bodyContext.arc(jointPoint.x, jointPoint.y, HANDSIZE, 0, Math.PI * 2, true);
        bodyContext.fill();
        bodyContext.closePath();
        bodyContext.globalAlpha = 1;
    }

    // Draws clipped edges
    var drawClippedEdges = function (body) {

        var clippedEdges = body.clippedEdges;

        bodyContext.fillStyle = "red";

        if (hasClippedEdges(clippedEdges, kinect.FrameEdges.bottom)) {
            bodyContext.fillRect(0, bodyCanvas.height - CLIPBOUNDSTHICKNESS, bodyCanvas.width, CLIPBOUNDSTHICKNESS);
        }

        if (hasClippedEdges(clippedEdges, kinect.FrameEdges.top)) {
            bodyContext.fillRect(0, 0, bodyCanvas.width, CLIPBOUNDSTHICKNESS);
        }

        if (hasClippedEdges(clippedEdges, kinect.FrameEdges.left)) {
            bodyContext.fillRect(0, 0, CLIPBOUNDSTHICKNESS, bodyCanvas.height);
        }

        if (hasClippedEdges(clippedEdges, kinect.FrameEdges.right)) {
            bodyContext.fillRect(bodyCanvas.width - CLIPBOUNDSTHICKNESS, 0, CLIPBOUNDSTHICKNESS, bodyCanvas.height);
        }
    }

    // Checks if an edge is clipped
    var hasClippedEdges = function (edges, clippedEdge) {
        return ((edges & clippedEdge) != 0);
    }

    // Allocate space for joint locations
    var createJointPoints = function () {
        var jointPoints = new Array();

        for (var i = 0; i < jointCount; ++i) {
            jointPoints.push({ joint: 0, x: 0, y: 0 });
        }

        return jointPoints;
    }

    // Create array of bones
    var populateBones = function () {
        var bones = new Array();

        // torso
        bones.push({ jointStart: kinect.JointType.head,             jointEnd: kinect.JointType.neck });
        bones.push({ jointStart: kinect.JointType.neck,             jointEnd: kinect.JointType.spineShoulder });
        bones.push({ jointStart: kinect.JointType.spineShoulder,    jointEnd: kinect.JointType.spineMid });
        bones.push({ jointStart: kinect.JointType.spineMid,         jointEnd: kinect.JointType.spineBase });
        bones.push({ jointStart: kinect.JointType.spineShoulder,    jointEnd: kinect.JointType.shoulderRight });
        bones.push({ jointStart: kinect.JointType.spineShoulder,    jointEnd: kinect.JointType.shoulderLeft });
        bones.push({ jointStart: kinect.JointType.spineBase,        jointEnd: kinect.JointType.hipRight });
        bones.push({ jointStart: kinect.JointType.spineBase,        jointEnd: kinect.JointType.hipLeft });

        // right arm
        bones.push({ jointStart: kinect.JointType.shoulderRight,    jointEnd: kinect.JointType.elbowRight });
        bones.push({ jointStart: kinect.JointType.elbowRight,       jointEnd: kinect.JointType.wristRight });
        bones.push({ jointStart: kinect.JointType.wristRight,       jointEnd: kinect.JointType.handRight });
        bones.push({ jointStart: kinect.JointType.handRight,        jointEnd: kinect.JointType.handTipRight });
        bones.push({ jointStart: kinect.JointType.wristRight,       jointEnd: kinect.JointType.thumbRight });

        // left arm
        bones.push({ jointStart: kinect.JointType.shoulderLeft,     jointEnd: kinect.JointType.elbowLeft });
        bones.push({ jointStart: kinect.JointType.elbowLeft,        jointEnd: kinect.JointType.wristLeft });
        bones.push({ jointStart: kinect.JointType.wristLeft,        jointEnd: kinect.JointType.handLeft });
        bones.push({ jointStart: kinect.JointType.handLeft,         jointEnd: kinect.JointType.handTipLeft });
        bones.push({ jointStart: kinect.JointType.wristLeft,        jointEnd: kinect.JointType.thumbLeft });

        // right leg
        bones.push({ jointStart: kinect.JointType.hipRight,         jointEnd: kinect.JointType.kneeRight });
        bones.push({ jointStart: kinect.JointType.kneeRight,        jointEnd: kinect.JointType.ankleRight });
        bones.push({ jointStart: kinect.JointType.ankleRight,       jointEnd: kinect.JointType.footRight });

        // left leg
        bones.push({ jointStart: kinect.JointType.hipLeft,          jointEnd: kinect.JointType.kneeLeft });
        bones.push({ jointStart: kinect.JointType.kneeLeft,         jointEnd: kinect.JointType.ankleLeft });
        bones.push({ jointStart: kinect.JointType.ankleLeft,        jointEnd: kinect.JointType.footLeft });

        return bones;
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

                // open the reader for frames
                bodyFrameReader = sensor.bodyFrameSource.openReader();

                // wire handler for frame arrival
                bodyFrameReader.addEventListener("framearrived", reader_BodyFrameArrived);

                // get depth frame description
                var depthFrameDescription = sensor.depthFrameSource.frameDescription;

                // create bodies array
                bodies = new Array(sensor.bodyFrameSource.bodyCount);

                // create bones
                bones = populateBones();

                // set number of joints and bones
                jointCount = kinect.Body.jointCount;
                boneCount = bones.length;

                // get canvas objects
                bodyCanvas = document.getElementById("mainCanvas");
                bodyCanvas.width = depthFrameDescription.width;;
                bodyCanvas.height = depthFrameDescription.height;;
                bodyContext = bodyCanvas.getContext("2d");

                // set body colors for each unique body
                bodyColors = [
                    "red",
                    "orange",
                    "green",
                    "blue",
                    "indigo",
                    "violet"
                ];

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
