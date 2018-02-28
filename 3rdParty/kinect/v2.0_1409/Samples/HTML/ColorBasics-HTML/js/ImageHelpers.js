//------------------------------------------------------------------------------
// <copyright file="ImageHelpers.js" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// Windows 8.1 is needed for WebGL support, and WebGL is desired for efficiency in rendering an image buffer with 1920x1080 pixels
function CanvasImageHelper() {
    "use strict";

    //////////////////////////////////////////////////////////////
    // ImageMetadata object constructor
    function ImageMetadata(imageCanvas) {
        
        //////////////////////////////////////////////////////////////
        // ImageMetadata private constants

        // vertices representing entire viewport as two triangles which make up the whole
        // rectangle, in post-projection/clipspace coordinates
        var VIEWPORT_VERTICES = new Float32Array([
            -1.0, -1.0,
            1.0, -1.0,
            -1.0, 1.0,
            -1.0, 1.0,
            1.0, -1.0,
            1.0, 1.0]);
        var NUM_VIEWPORT_VERTICES = VIEWPORT_VERTICES.length / 2;
        
        // Texture coordinates corresponding to each viewport vertex
        var VERTEX_TEXTURE_COORDS = new Float32Array([
            0.0, 1.0,
            1.0, 1.0,
            0.0, 0.0,
            0.0, 0.0,
            1.0, 1.0,
            1.0, 0.0]);

        //////////////////////////////////////////////////////////////
        // ImageMetadata private properties
        var metadata = this;
        var contextAttributes = { premultipliedAlpha: true };
        var glContext = imageCanvas.getContext('webgl', contextAttributes) || imageCanvas.getContext('experimental-webgl', contextAttributes);
        glContext.clearColor(0.0, 0.0, 0.0, 0.0);      // Set clear color to black, fully transparent
        
        var vertexShader = createShaderFromSource(glContext.VERTEX_SHADER,
            "\
            attribute vec2 aPosition;\
            attribute vec2 aTextureCoord;\
            \
            varying highp vec2 vTextureCoord;\
            \
            void main() {\
                gl_Position = vec4(aPosition, 0, 1);\
                vTextureCoord = aTextureCoord;\
            }");
        var fragmentShader = createShaderFromSource(glContext.FRAGMENT_SHADER,
            "\
            precision mediump float;\
            \
            varying highp vec2 vTextureCoord;\
            \
            uniform sampler2D uSampler;\
            \
            void main() {\
                gl_FragColor = texture2D(uSampler, vTextureCoord);\
            }");
        var program = createProgram([vertexShader, fragmentShader]);
        glContext.useProgram(program);

        var positionAttribute = glContext.getAttribLocation(program, "aPosition");
        glContext.enableVertexAttribArray(positionAttribute);

        var textureCoordAttribute = glContext.getAttribLocation(program, "aTextureCoord");
        glContext.enableVertexAttribArray(textureCoordAttribute);
        
        // Associate the uniform texture sampler with TEXTURE0 slot
        var textureSamplerUniform = glContext.getUniformLocation(program, "uSampler");
        glContext.uniform1i(textureSamplerUniform, 0);

        // Create a buffer used to represent whole set of viewport vertices
        var vertexBuffer = glContext.createBuffer();
        glContext.bindBuffer(glContext.ARRAY_BUFFER, vertexBuffer);
        glContext.bufferData(glContext.ARRAY_BUFFER, VIEWPORT_VERTICES, glContext.STATIC_DRAW);
        glContext.vertexAttribPointer(positionAttribute, 2, glContext.FLOAT, false, 0, 0);
        
        // Create a buffer used to represent whole set of vertex texture coordinates
        var textureCoordinateBuffer = glContext.createBuffer();
        glContext.bindBuffer(glContext.ARRAY_BUFFER, textureCoordinateBuffer);
        glContext.bufferData(glContext.ARRAY_BUFFER, VERTEX_TEXTURE_COORDS, glContext.STATIC_DRAW);
        glContext.vertexAttribPointer(textureCoordAttribute, 2, glContext.FLOAT, false, 0, 0);

        // Create a texture to contain images from Kinect server
        // Note: TEXTURE_MIN_FILTER, TEXTURE_WRAP_S and TEXTURE_WRAP_T parameters need to be set
        //       so we can handle textures whose width and height are not a power of 2.
        var texture = glContext.createTexture();
        glContext.bindTexture(glContext.TEXTURE_2D, texture);
        glContext.texParameteri(glContext.TEXTURE_2D, glContext.TEXTURE_MAG_FILTER, glContext.LINEAR);
        glContext.texParameteri(glContext.TEXTURE_2D, glContext.TEXTURE_MIN_FILTER, glContext.LINEAR);
        glContext.texParameteri(glContext.TEXTURE_2D, glContext.TEXTURE_WRAP_S, glContext.CLAMP_TO_EDGE);
        glContext.texParameteri(glContext.TEXTURE_2D, glContext.TEXTURE_WRAP_T, glContext.CLAMP_TO_EDGE);
        glContext.bindTexture(glContext.TEXTURE_2D, null);
        
        // Since we're only using one single texture, we just make TEXTURE0 the active one
        // at all times
        glContext.activeTexture(glContext.TEXTURE0);

        //////////////////////////////////////////////////////////////
        // ImageMetadata private methods

        // Create a shader of specified type, with the specified source, and compile it.
        //     .createShaderFromSource(shaderType, shaderSource)
        //
        // shaderType: Type of shader to create (fragment or vertex shader)
        // shaderSource: Source for shader to create (string)
        function createShaderFromSource(shaderType, shaderSource) {
            var shader = glContext.createShader(shaderType);
            glContext.shaderSource(shader, shaderSource);
            glContext.compileShader(shader);

            // Check for errors during compilation
            var status = glContext.getShaderParameter(shader, glContext.COMPILE_STATUS);
            if (!status) {
                var infoLog = glContext.getShaderInfoLog(shader);
                console.log("Unable to compile Kinect '" + shaderType + "' shader. Error:" + infoLog);
                glContext.deleteShader(shader);
                return null;
            }

            return shader;
        }
        
        // Create a WebGL program attached to the specified shaders.
        //     .createProgram(shaderArray)
        //
        // shaderArray: Array of shaders to attach to program
        function createProgram(shaderArray) {
            var newProgram = glContext.createProgram();
            
            for (var shaderIndex = 0; shaderIndex < shaderArray.length; ++shaderIndex) {
                glContext.attachShader(newProgram, shaderArray[shaderIndex]);
            }
            glContext.linkProgram(newProgram);
            
            // Check for errors during linking
            var status = glContext.getProgramParameter(newProgram, glContext.LINK_STATUS);
            if (!status) {
                var infoLog = glContext.getProgramInfoLog(newProgram);
                console.log("Unable to link Kinect WebGL program. Error:" + infoLog);
                glContext.deleteProgram(newProgram);
                return null;
            }

            return newProgram;
        }

        //////////////////////////////////////////////////////////////
        // ImageMetadata public properties
        this.isProcessing = false;
        this.canvas = imageCanvas;
        this.width = 0;
        this.height = 0;
        this.gl = glContext;
        
        //////////////////////////////////////////////////////////////
        // ImageMetadata public functions
        
        // Draw image data into WebGL canvas context
        //     .processImageData(imageBuffer, width, height)
        //
        // imageBuffer: ArrayBuffer containing image data
        // width: width of image corresponding to imageBuffer data
        // height: height of image corresponding to imageBuffer data
        this.processImageData = function(imageBuffer, width, height) {
            if ((width != metadata.width) || (height != metadata.height)) {
                // Whenever the image width or height changes, update tracked metadata and canvas
                // viewport dimensions.
                this.width = width;
                this.height = height;
                this.canvas.width = width;
                this.canvas.height = height;
                glContext.viewport(0, 0, width, height);
            }
            //Associate the specified image data with a WebGL texture so that shaders can use it
            glContext.bindTexture(glContext.TEXTURE_2D, texture);
            glContext.texImage2D(glContext.TEXTURE_2D, 0, glContext.RGBA, width, height, 0, glContext.RGBA, glContext.UNSIGNED_BYTE, new Uint8Array(imageBuffer));

            //Draw vertices so that shaders get invoked and we can render the texture
            glContext.drawArrays(glContext.TRIANGLES, 0, NUM_VIEWPORT_VERTICES);
            glContext.bindTexture(glContext.TEXTURE_2D, null);
        };

        // Clear all image data from WebGL canvas
        //     .clear()
        this.clear = function() {
            glContext.clear(glContext.COLOR_BUFFER_BIT | glContext.DEPTH_BUFFER_BIT);
        };
    }

    //////////////////////////////////////////////////////////////
    // CanvasImageHelper private properties
    var imageMetadata = null;

    //////////////////////////////////////////////////////////////
    // CanvasImageHelper private functions
    
    // Associate helper with canvas.
    //     .setCanvas(canvas)
    //
    // canvas: Canvas to bind to user viewer stream
    function setCanvas(canvas) {
        if (canvas != null) {
            var metadata = new ImageMetadata(canvas);
            imageMetadata = metadata;
        } else if (imageMetadata != null) {
            // If specified canvas is null but we're already tracking image data,
            // remove metadata associated with this helper.
            imageMetadata = null;
        }
    }

    //////////////////////////////////////////////////////////////
    // CanvasImageHelper public functions

    // Bind the specified canvas element with this helper
    //     .bindToCanvas( canvas )
    // 
    // canvas: Canvas to bind with this helper
    this.bindToCanvas = function (canvas) {
        if (!(canvas instanceof HTMLCanvasElement)) {
            throw new Error("first parameter must be specified and must be a canvas element");
        }

        this.unbindFromCanvas();

        setCanvas(canvas);
    };

    // Unbind the CanvasImageHelper from previously bound canvas element, if any.
    //     .unbindFromCanvas()
    this.unbindFromCanvas = function (streamName) {
        setCanvas(null);
    };

    // Send image data to be processed by WebGL canvas context
    //     .processImageData(imageBuffer, width, height)
    //
    // imageBuffer: ArrayBuffer containing image data
    // width: width of image corresponding to imageBuffer data
    // height: height of image corresponding to imageBuffer data
    this.processImageData = function (imageBuffer, width, height) {
        if (imageMetadata == null) {
            // We're not bound to a canvas, so no work to do
            return;
        }

        if (imageMetadata.isProcessing || (width <= 0) || (height <= 0)) {
            // Don't start processing new data when we are in the middle of
            // processing data already.
            // Also, Only do work if image data to process is of the expected size
            return;
        }

        imageMetadata.isProcessing = true;
        imageMetadata.processImageData(imageBuffer, width, height);
        imageMetadata.isProcessing = false;
    }

    // Get data manager used to interact with canvas
    //     .getDataManager()
    this.getDataManager = function () {
        return imageMetadata;
    };
}

// Create a circular buffer pool that manages a fixed number of buffers
// that can be checked out, used, and checked back in.
//     ImageBufferManager(bufferSize [, numBuffers])
//
// bufferSize: Size of each buffer
// numBuffers: [Optional]Number of buffers in buffer pool
function ImageBufferManager(bufferSize, numBuffers) {
    var DEFAULT_NUM_BUFFERS = 2;

    if ((bufferSize == null) || (typeof bufferSize != 'number')) {
        throw new Error("First parameter must be a number");
    }

    if ((numBuffers != null) && (typeof numBuffers != 'number')) {
        throw new Error("Second parameter must be a number or left unspecified");
    }

    if ((numBuffers == null) || (numBuffers < 1)) {
        numBuffers = DEFAULT_NUM_BUFFERS;
    }

    // Buffer object that gets handed back to clients when they do an "acquire" call
    function PublicBuffer(metadata) {

        //////////////////////////////////////////////////////////////
        // PublicBuffer public properties
        this.buffer = metadata.buffer;

        //////////////////////////////////////////////////////////////
        // PublicBuffer public methods

        // Asynchronously transfer buffer data to an ArrayBuffer object.
        //     .transferToArrayBufferAsync()
        //
        // Returns a promise to deliver ann ArrayBuffer object asynchronously.
        this.transferToArrayBufferAsync = function () {
            metadata.memoryStream.seek(0);
            var promise = metadata.memoryStream.writeAsync(metadata.buffer);

            return promise.then(function () {
                if (metadata.inputStream != null) {
                    metadata.inputStream.close();
                }

                metadata.inputStream = metadata.memoryStream.getInputStreamAt(0);
                var msStream = MSApp.createStreamFromInputStream('application/octet-stream', metadata.inputStream);
                return new WinJS.Promise(function (completeDispatch, errorDispatch, progressDispatch) {
                    var streamReader = new MSStreamReader();
                    streamReader.onloadend = function (loadedEvent) {
                        completeDispatch(loadedEvent.currentTarget.result);
                    };
                    streamReader.onerror = function () {
                        errorDispatch(WinJS.ErrorFromName("Error while reading color buffer"));
                    };
                    streamReader.readAsArrayBuffer(msStream);
                });
            });
        };

        // Return this buffer to buffer pool.
        //     .close()
        this.close = function () {
            closeBuffer(this);
        };
    }

    // Metadata used for tracking buffers internally
    function BufferMetadata() {
        //////////////////////////////////////////////////////////////
        // BufferMetadata private properties
        this.buffer = new Windows.Storage.Streams.Buffer(bufferSize);
        this.memoryStream = new Windows.Storage.Streams.InMemoryRandomAccessStream();
        this.inputStream = null;
        this.publicBuffer = new PublicBuffer(this);
        this.isAvailable = true;
    }

    //////////////////////////////////////////////////////////////
    // ImageBufferManager private properties
    var buffers = [];
    for (var i = 0; i < numBuffers; ++i) {
        buffers.push(new BufferMetadata());
    }

    //////////////////////////////////////////////////////////////
    // ImageBufferManager private functions

    function closeBuffer(returnedBuffer) {
        for (var i = 0; i < numBuffers; ++i) {
            if (buffers[i].publicBuffer === returnedBuffer) {
                buffers[i].isAvailable = true;
                if (buffers[i].inputStream != null) {
                    buffers[i].inputStream.close();
                    buffers[i].inputStream = null;
                }
                break;
            }
        }
    }

    //////////////////////////////////////////////////////////////
    // ImageBufferManager public functions

    // Check out a buffer from buffer pool
    //     .acquireBuffer()
    // Returns a buffer from the pool, if one is available.
    // Returns null otherwise.
    this.acquireBuffer = function () {
        var buffer = null;

        // See if there is an available buffer and, if so, check it out
        for (var i = 0; i < numBuffers; ++i) {
            if (buffers[i].isAvailable) {
                buffer = buffers[i].publicBuffer;
                buffers[i].isAvailable = false;
                if (buffers[i].inputStream != null) {
                    buffers[i].inputStream.close();
                    buffers[i].inputStream = null;
                }
                break;
            }
        }

        return buffer;
    };
}
