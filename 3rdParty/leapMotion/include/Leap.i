////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.               /
// Leap Motion proprietary and confidential. Not for distribution.              /
// Use subject to the terms of the Leap Motion SDK Agreement available at       /
// https://developer.leapmotion.com/sdk_agreement, or another agreement         /
// between Leap Motion and you, your company or other organization.             /
////////////////////////////////////////////////////////////////////////////////

// usage:
//   swig -c++ -python -o LeapPython.cpp -interface LeapPython Leap.i
//   swig -c++ -java   -o LeapJava.cpp -package com.leapmotion.leap -outdir com/leapmotion/leap Leap.i
//   swig -c++ -csharp -o LeapCSharp.cpp -dllimport LeapCSharp -namespace Leap Leap.i

%module(directors="1", threads="1") Leap
#pragma SWIG nowarn=325

%include "std_string.i"
%include "std_vector.i"
%include "stdint.i"
%include "attribute.i"

////////////////////////////////////////////////////////////////////////////////
// Ignore constructors for internal use only                                    /
////////////////////////////////////////////////////////////////////////////////

%ignore Leap::Pointable::Pointable(PointableImplementation*);
%ignore Leap::Pointable::Pointable(FingerImplementation*);
%ignore Leap::Pointable::Pointable(ToolImplementation*);
%ignore Leap::Finger::Finger(FingerImplementation*);
%ignore Leap::Tool::Tool(ToolImplementation*);
%ignore Leap::Bone::Bone(BoneImplementation*);
%ignore Leap::Hand::Hand(HandImplementation*);
%ignore Leap::Arm::Arm(HandImplementation*);
%ignore Leap::Gesture::Gesture(GestureImplementation*);
%ignore Leap::Image::Image(ImageImplementation*);
%ignore Leap::Frame::Frame(FrameImplementation*);
%ignore Leap::Controller::Controller(ControllerImplementation*);
%ignore Leap::Device::Device(DeviceImplementation*);
%ignore Leap::FailedDevice::FailedDevice(FailedDeviceImplementation*);
%ignore Leap::InteractionBox::InteractionBox(InteractionBoxImplementation*);
%ignore Leap::BugReport::BugReport(BugReportImplementation*);

/////////////////////////////////////////////////////////////////////////////////////
// Set Attributes (done after functions are uppercased, but before vars are lowered) /
/////////////////////////////////////////////////////////////////////////////////////
//TODO: If possible, figure out how to auomatically make any C++ function
// that is const and takes no arguments be defined as a property in C/

%define %constattrib( Class, Type, Name )
  %attribute( Class, Type, Name, Name )
%enddef

%define %staticattrib(Class, AttributeType, AttributeName)
  %ignore Class::AttributeName();
  %ignore Class::AttributeName() const;
  %immutable Class::AttributeName;
  %extend Class {
    AttributeType AttributeName;
  }
  %{
    #define %mangle(Class) ##_## AttributeName ## _get() Class::AttributeName()
  %}
%enddef

%define %leapattrib( Class, Type, Name )
  %attributeval(Class, Leap::Type, Name, Name)
%enddef

#if SWIGCSHARP || SWIGPYTHON

%rename(GestureType) Leap::Gesture::Type;
%rename(GestureState) Leap::Gesture::State;
%rename(FingerJoint) Leap::Finger::Joint;
%rename(FingerType) Leap::Finger::Type;
%rename(BoneType) Leap::Bone::Type;
%rename(DeviceType) Leap::Device::Type;
%rename(FailureType) Leap::FailedDevice::Failure;

#endif

// Apply language specific caseing
#if SWIGCSHARP

%rename("%(camelcase)s", %$not %$isenumitem) "";

#elif SWIGPYTHON

%typemap(varout, noblock=1) SWIGTYPE & {
  %set_varoutput(SWIG_NewPointerObj(%as_voidptr(&$1()), $descriptor, %newpointer_flags));
}

%rename("%(undercase)s", notregexmatch$name="^[A-Z0-9_]+$") "";

#endif

#if SWIGCSHARP || SWIGPYTHON

%constattrib( Leap::Pointable, int, id );
%leapattrib( Leap::Pointable, Hand, hand );
%leapattrib( Leap::Pointable, Vector, tipPosition );
%leapattrib( Leap::Pointable, Vector, tipVelocity );
%leapattrib( Leap::Pointable, Vector, direction );
%constattrib( Leap::Pointable, float, width );
%constattrib( Leap::Pointable, float, length );
%constattrib( Leap::Pointable, bool, isTool );
%constattrib( Leap::Pointable, bool, isFinger );
%constattrib( Leap::Pointable, bool, isExtended );
%constattrib( Leap::Pointable, bool, isValid );
%constattrib( Leap::Pointable, Leap::Pointable::Zone, touchZone )
%constattrib( Leap::Pointable, float, touchDistance )
%leapattrib( Leap::Pointable, Vector, stabilizedTipPosition )
%constattrib( Leap::Pointable, float, timeVisible );
%leapattrib( Leap::Pointable, Frame, frame );

%constattrib( Leap::Finger, Leap::Finger::Type, type )

%leapattrib( Leap::Bone, Vector, prevJoint );
%leapattrib( Leap::Bone, Vector, nextJoint );
%leapattrib( Leap::Bone, Vector, center );
%leapattrib( Leap::Bone, Vector, direction );
%constattrib( Leap::Bone, float, length );
%constattrib( Leap::Bone, float, width );
%constattrib( Leap::Bone, Leap::Bone::Type, type )
%leapattrib( Leap::Bone, Matrix, basis )
%constattrib( Leap::Bone, bool, isValid );

%constattrib( Leap::Hand, int, id );
%leapattrib( Leap::Hand, PointableList, pointables );
%leapattrib( Leap::Hand, FingerList, fingers );
%leapattrib( Leap::Hand, Vector, palmPosition );
%leapattrib( Leap::Hand, Vector, palmVelocity );
%leapattrib( Leap::Hand, Vector, palmNormal );
%leapattrib( Leap::Hand, Vector, direction );
%leapattrib( Leap::Hand, Matrix, basis )
%constattrib( Leap::Hand, bool, isValid );
%leapattrib( Leap::Hand, Vector, sphereCenter );
%constattrib( Leap::Hand, float, sphereRadius );
%constattrib( Leap::Hand, float, grabAngle );
%constattrib( Leap::Hand, float, pinchDistance );
%constattrib( Leap::Hand, float, grabStrength );
%constattrib( Leap::Hand, float, pinchStrength );
%constattrib( Leap::Hand, float, palmWidth );
%leapattrib( Leap::Hand, Vector, stabilizedPalmPosition )
%leapattrib( Leap::Hand, Vector, wristPosition )
%constattrib( Leap::Hand, float, timeVisible );
%constattrib( Leap::Hand, float, confidence );
%constattrib( Leap::Hand, bool, isLeft );
%constattrib( Leap::Hand, bool, isRight );
%leapattrib( Leap::Hand, Frame, frame );
%leapattrib( Leap::Hand, Arm, arm );

%constattrib( Leap::Arm, float, width );
%leapattrib( Leap::Arm, Vector, center );
%leapattrib( Leap::Arm, Vector, direction );
%leapattrib( Leap::Arm, Matrix, basis )
%leapattrib( Leap::Arm, Vector, elbowPosition );
%leapattrib( Leap::Arm, Vector, wristPosition );
%constattrib( Leap::Arm, bool, isValid );

%constattrib( Leap::Gesture, Leap::Gesture::Type, type )
%constattrib( Leap::Gesture, Leap::Gesture::State, state )
%constattrib( Leap::Gesture, int32_t, id );
%constattrib( Leap::Gesture, int64_t, duration );
%constattrib( Leap::Gesture, float, durationSeconds );
%leapattrib( Leap::Gesture, Frame, frame );
%leapattrib( Leap::Gesture, HandList, hands );
%leapattrib( Leap::Gesture, PointableList, pointables );
%constattrib( Leap::Gesture, bool, isValid );
%leapattrib( Leap::CircleGesture, Vector, center );
%leapattrib( Leap::CircleGesture, Vector, normal );
%constattrib( Leap::CircleGesture, float, progress );
%constattrib( Leap::CircleGesture, float, radius );
%leapattrib( Leap::CircleGesture, Pointable, pointable );
%leapattrib( Leap::SwipeGesture, Vector, startPosition );
%leapattrib( Leap::SwipeGesture, Vector, position );
%leapattrib( Leap::SwipeGesture, Vector, direction );
%constattrib( Leap::SwipeGesture, float, speed );
%leapattrib( Leap::SwipeGesture, Pointable, pointable );
%leapattrib( Leap::ScreenTapGesture, Vector, position );
%leapattrib( Leap::ScreenTapGesture, Vector, direction );
%constattrib( Leap::ScreenTapGesture, float, progress );
%leapattrib( Leap::ScreenTapGesture, Pointable, pointable );
%leapattrib( Leap::KeyTapGesture, Vector, position );
%leapattrib( Leap::KeyTapGesture, Vector, direction );
%constattrib( Leap::KeyTapGesture, float, progress );
%leapattrib( Leap::KeyTapGesture, Pointable, pointable );

%constattrib( Leap::Image, int64_t, sequenceId );
%constattrib( Leap::Image, int32_t, id );
%constattrib( Leap::Image, int, width );
%constattrib( Leap::Image, int, height );
%constattrib( Leap::Image, int, bytesPerPixel );
%constattrib( Leap::Image, Leap::Image::FormatType, format );
%constattrib( Leap::Image, int, distortionWidth );
%constattrib( Leap::Image, int, distortionHeight );
%constattrib( Leap::Image, float, rayOffsetX );
%constattrib( Leap::Image, float, rayOffsetY );
%constattrib( Leap::Image, float, rayScaleX );
%constattrib( Leap::Image, float, rayScaleY );
%constattrib( Leap::Image, int64_t, timestamp );
%constattrib( Leap::Image, bool, isValid );

// Count is made a const attribute in C# but renamed to __len__ in Python
#if SWIGCSHARP
%constattrib( Leap::PointableList, int, count );
%constattrib( Leap::FingerList, int, count );
%constattrib( Leap::ToolList, int, count );
%constattrib( Leap::HandList, int, count );
%constattrib( Leap::GestureList, int, count );
%constattrib( Leap::ImageList, int, count );
%constattrib( Leap::DeviceList, int, count );
%constattrib( Leap::FailedDeviceList, int, count );
#endif

%constattrib( Leap::PointableList, bool, isEmpty );
%constattrib( Leap::FingerList, bool, isEmpty );
%constattrib( Leap::ToolList, bool, isEmpty );
%constattrib( Leap::HandList, bool, isEmpty );
%constattrib( Leap::GestureList, bool, isEmpty );
%constattrib( Leap::ImageList, bool, isEmpty );
%constattrib( Leap::DeviceList, bool, isEmpty );
%constattrib( Leap::FailedDeviceList, bool, isEmpty );

%leapattrib( Leap::PointableList, Pointable, leftmost );
%leapattrib( Leap::PointableList, Pointable, rightmost );
%leapattrib( Leap::PointableList, Pointable, frontmost );
%leapattrib( Leap::FingerList, Finger, leftmost );
%leapattrib( Leap::FingerList, Finger, rightmost );
%leapattrib( Leap::FingerList, Finger, frontmost );
%leapattrib( Leap::ToolList, Tool, leftmost );
%leapattrib( Leap::ToolList, Tool, rightmost );
%leapattrib( Leap::ToolList, Tool, frontmost );
%leapattrib( Leap::HandList, Hand, leftmost );
%leapattrib( Leap::HandList, Hand, rightmost );
%leapattrib( Leap::HandList, Hand, frontmost );

%constattrib( Leap::Frame, int64_t, id );
%constattrib( Leap::Frame, int64_t, timestamp );
%constattrib( Leap::Frame, float, currentFramesPerSecond );
%leapattrib( Leap::Frame, PointableList, pointables );
%leapattrib( Leap::Frame, FingerList, fingers );
%leapattrib( Leap::Frame, ToolList, tools );
%leapattrib( Leap::Frame, HandList, hands );
%leapattrib( Leap::Frame, ImageList, images );
%leapattrib( Leap::Frame, ImageList, rawImages );
%constattrib( Leap::Frame, bool, isValid );
%leapattrib( Leap::Frame, InteractionBox, interactionBox );
%constattrib( Leap::Frame, int, serializeLength );

%constattrib( Leap::Device, float, horizontalViewAngle );
%constattrib( Leap::Device, float, verticalViewAngle );
%constattrib( Leap::Device, float, range );
%constattrib( Leap::Device, float, baseline );
%constattrib( Leap::Device, bool, isValid );
%constattrib( Leap::Device, bool, isEmbedded );
%constattrib( Leap::Device, bool, isStreaming );
%constattrib( Leap::Device, bool, isSmudged );
%constattrib( Leap::Device, bool, isLightingBad );

%constattrib( Leap::Device, Leap::Device::Type, type );
%attributestring( Leap::Device, std::string, serialNumber, serialNumber );
%leapattrib( Leap::Device, Vector, position );
%leapattrib( Leap::Device, Matrix, orientation );

%attributestring( Leap::FailedDevice, std::string, pnpId, pnpId);
%constattrib( Leap::FailedDevice, Leap::FailedDevice::FailureType, failure);

%leapattrib( Leap::InteractionBox, Vector, center );
%constattrib( Leap::InteractionBox, float, width );
%constattrib( Leap::InteractionBox, float, height );
%constattrib( Leap::InteractionBox, float, depth );
%constattrib( Leap::InteractionBox, bool, isValid );

%constattrib( Leap::BugReport, bool, isActive );
%constattrib( Leap::BugReport, float, progress );
%constattrib( Leap::BugReport, float, duration );

#if SWIGCSHARP
%csmethodmodifiers Leap::Finger::invalid "public new";
%csmethodmodifiers Leap::Tool::invalid "public new";
#endif
%staticattrib( Leap::Pointable, static const Pointable&, invalid);
%staticattrib( Leap::Finger, static const Finger&, invalid);
%staticattrib( Leap::Tool, static const Tool&, invalid);
%staticattrib( Leap::Bone, static const Bone&, invalid);
%staticattrib( Leap::Hand, static const Hand&, invalid);
%staticattrib( Leap::Arm, static const Arm&, invalid);
%staticattrib( Leap::Gesture, static const Gesture&, invalid);
%staticattrib( Leap::Image, static const Image&, invalid);
%staticattrib( Leap::Device, static const Device&, invalid );
%staticattrib( Leap::InteractionBox, static const InteractionBox&, invalid );
%staticattrib( Leap::Frame, static const Frame&, invalid);

%constattrib( Leap::Vector, float, magnitude );
%constattrib( Leap::Vector, float, magnitudeSquared );
%constattrib( Leap::Vector, float, pitch );
%constattrib( Leap::Vector, float, roll );
%constattrib( Leap::Vector, float, yaw );
%leapattrib( Leap::Vector, Vector, normalized );

%constattrib( Leap::Controller, bool, isConnected );
%constattrib( Leap::Controller, bool, hasFocus );
%constattrib( Leap::Controller, Controller::PolicyFlag, policyFlags );
%leapattrib( Leap::Controller, Config, config );
%leapattrib( Leap::Controller, ImageList, images );
%leapattrib( Leap::Controller, ImageList, rawImages );
%leapattrib( Leap::Controller, DeviceList, devices );
%leapattrib( Leap::Controller, BugReport, bugReport );

%staticattrib( Leap::Vector, static const Vector&, zero );
%staticattrib( Leap::Vector, static const Vector&, xAxis );
%staticattrib( Leap::Vector, static const Vector&, yAxis );
%staticattrib( Leap::Vector, static const Vector&, zAxis );
%staticattrib( Leap::Vector, static const Vector&, forward );
%staticattrib( Leap::Vector, static const Vector&, backward );
%staticattrib( Leap::Vector, static const Vector&, left );
%staticattrib( Leap::Vector, static const Vector&, right );
%staticattrib( Leap::Vector, static const Vector&, up );
%staticattrib( Leap::Vector, static const Vector&, down );

%staticattrib( Leap::Matrix, static const Matrix&, identity );

#endif

%ignore Leap::Frame::serialize() const;
%ignore Leap::Frame::deserialize(const std::string&);
%ignore Leap::Image::data() const;
%ignore Leap::Image::distortion() const;

#if !defined(SWIGCSHARP) && !defined(SWIGPYTHON)

%ignore Leap::Image::dataPointer() const;
%ignore Leap::Image::distortionPointer() const;

#endif

#if SWIGCSHARP

%include "arrays_csharp.i"
%apply unsigned char INOUT[] { unsigned char* };
%apply float OUTPUT[] { float* };
%typemap(cstype) void* "System.IntPtr"
%typemap(csout) void* { return $imcall; }

%rename(SerializeWithArg) Leap::Frame::serialize(unsigned char*) const;
%rename(DeserializeWithLength) Leap::Frame::deserialize(const unsigned char*, int length);

%rename("%(lowercamelcase)s", %$isvariable) "";
%ignore Leap::DEG_TO_RAD;
%ignore Leap::RAD_TO_DEG;
%ignore Leap::PI;

SWIG_CSBODY_PROXY(public, public, SWIGTYPE)

%ignore Leap::Image::data(unsigned char*) const;
%ignore Leap::Image::distortion(float*) const;

%typemap(cscode) Leap::Image %{
  /**
  * The image data.
  *
  * The image data is a set of 8-bit intensity values. The buffer is
  * ``image.Width * image.Height * image.BytesPerPixel`` bytes long.
  *
  * \include Image_data_1.txt
  *
  * @since 2.1.0
  */
  public byte[] Data {
    get {
      byte[] ret = new byte[Width * Height * BytesPerPixel];
      DataWithArg(ret);
      return ret;
    }
  }
  public void DataWithArg(byte[] dst) {
    System.Runtime.InteropServices.Marshal.Copy(DataPointer(), dst, 0, Width * Height * BytesPerPixel);
  }
  /**
  * The distortion calibration map for this image.
  *
  * The calibration map is a 64x64 grid of points. Each point is defined by
  * a pair of 32-bit floating point values. Each point in the map
  * represents a ray projected into the camera. The value of
  * a grid point defines the pixel in the image data containing the brightness
  * value produced by the light entering along the corresponding ray. By
  * interpolating between grid data points, you can find the brightness value
  * for any projected ray. Grid values that fall outside the range [0..1] do
  * not correspond to a value in the image data and those points should be ignored.
  *
  * \include Image_distortion_1.txt
  *
  * The calibration map can be used to render an undistorted image as well as to
  * find the true angle from the camera to a feature in the raw image. The
  * distortion map itself is designed to be used with GLSL shader programs.
  * In other contexts, it may be more convenient to use the Image Rectify()
  * and Warp() functions.
  *
  * Distortion is caused by the lens geometry as well as imperfections in the
  * lens and sensor window. The calibration map is created by the calibration
  * process run for each device at the factory (and which can be rerun by the
  * user).
  *
  * Note, in a future release, there will be two distortion maps per image;
  * one containing the horizontal values and the other containing the vertical values.
  *
  * @since 2.1.0
  */
  public float[] Distortion {
    get {
      float[] ret = new float[DistortionWidth * DistortionHeight];
      DistortionWithArg(ret);
      return ret;
    }
  }
  public void DistortionWithArg(float[] dst) {
    System.Runtime.InteropServices.Marshal.Copy(DistortionPointer(), dst, 0, DistortionWidth * DistortionHeight);
  }
%}

%typemap(cscode) Leap::Frame %{
  /**
  * Encodes this Frame object as a byte string.
  *
  * \include Frame_serialize.txt
  *
  * @since 2.1.0
  */
  public byte[] Serialize {
    get {
      byte[] ptr = new byte[SerializeLength];
      SerializeWithArg(ptr);
      return ptr;
    }
  }

 /**
  * Decodes a byte string to replace the properties of this Frame.
  *
  * A Controller object must be instantiated for this function to succeed, but
  * it does not need to be connected. To extract gestures from the deserialized
  * frame, you must enable the appropriate gestures first.
  *
  * Any existing data in the frame is
  * destroyed. If you have references to
  * child objects (hands, fingers, etc.), these are preserved as long as the
  * references remain in scope.
  *
  * \include Frame_deserialize.txt
  *
  * **Note:** The behavior when calling functions which take
  * another Frame object as a parameter is undefined when either frame has
  * been deserialized. For example, calling ``Gestures(sinceFrame)`` on a
  * deserialized frame or with a deserialized frame as parameter (or both)
  * does not necessarily return all gestures that occurred between the two
  * frames. Motion functions, like ``ScaleFactor(startFrame)``, are more
  * likely to return reasonable results, but could return anomalous values
  * in some cases.
  *
  * @param arg A byte array containing the bytes of a serialized frame.
  * @since 2.1.0
  */
  public void Deserialize(byte[] arg) {
    DeserializeWithLength(arg, arg.Length);
  }
%}

#elif SWIGPYTHON

%include "carrays.i"
%array_class(unsigned char, byteArray)
%array_class(float, floatArray)

%extend Leap::Image {
%pythoncode {
  def data(self):
      ptr = byte_array(self.width * self.height * self.bytes_per_pixel)
      LeapPython.Image_data(self, ptr)
      return ptr
  def distortion(self):
      ptr = float_array(self.distortion_width * self.distortion_height)
      LeapPython.Image_distortion(self, ptr)
      return ptr
  __swig_getmethods__["data"] = data
  if _newclass:data = _swig_property(data)
  __swig_getmethods__["distortion"] = distortion
  if _newclass:distortion = _swig_property(distortion)
}}

%extend Leap::Frame {
%pythoncode {
  def serialize(self):
      length = self.serialize_length
      str = byte_array(length)
      LeapPython.Frame_serialize(self, str)
      return (str, length)
  def deserialize(self, tup):
      LeapPython.Frame_deserialize(self, tup[0], tup[1])
  __swig_getmethods__["serialize"] = serialize
  if _newclass:serialize = _swig_property(serialize)
}}

%constattrib( Leap::Image, void*, dataPointer );
%constattrib( Leap::Image, void*, distortionPointer );

%rename("%(camelcase)s", %$isclass) "";
%rename("%(camelcase)s", %$isconstructor) "";

#elif SWIGJAVA

%include "arrays_java.i"
%apply signed char[] { unsigned char* };
%apply float[] { float* };

%typemap(javacode) Leap::Image %{
  /**
  * The image data.
  *
  * The image data is a set of 8-bit intensity values. The buffer is
  * ``image.width() * image.height() * image.bytesPerPixel()`` bytes long.
  *
  * \include Image_data_1.txt
  *
  * @since 2.1.0
  */
  public byte[] data() {
    byte[] ptr = new byte[width() * height() * bytesPerPixel()];
    LeapJNI.Image_data(swigCPtr, this, ptr);
    return ptr;
  }
  /**
  * The distortion calibration map for this image.
  *
  * The calibration map is a 64x64 grid of points. Each point is defined by
  * a pair of 32-bit floating point values. Each point in the map
  * represents a ray projected into the camera. The value of
  * a grid point defines the pixel in the image data containing the brightness
  * value produced by the light entering along the corresponding ray. By
  * interpolating between grid data points, you can find the brightness value
  * for any projected ray. Grid values that fall outside the range [0..1] do
  * not correspond to a value in the image data and those points should be ignored.
  *
  * \include Image_distortion_1.txt
  *
  * The calibration map can be used to render an undistorted image as well as to
  * find the true angle from the camera to a feature in the raw image. The
  * distortion map itself is designed to be used with GLSL shader programs.
  * In other contexts, it may be more convenient to use the Image rectify()
  * and warp() functions.
  *
  * Distortion is caused by the lens geometry as well as imperfections in the
  * lens and sensor window. The calibration map is created by the calibration
  * process run for each device at the factory (and which can be rerun by the
  * user).
  *
  * @returns The float array containing the camera lens calibration map.
  * @since 2.1.0
  */
  public float[] distortion() {
    float[] ptr = new float[distortionWidth() * distortionHeight()];
    LeapJNI.Image_distortion(swigCPtr, this, ptr);
    return ptr;
  }
%}

%typemap(javacode) Leap::Frame %{
 /**
  * Encodes this Frame object as a byte string.
  *
  * \include Frame_serialize.txt
  *
  * @returns The byte array encoding the data for this frame.
  * @since 2.1.0
  */
  public byte[] serialize() {
    byte[] ptr = new byte[serializeLength()];
    LeapJNI.Frame_serialize(swigCPtr, this, ptr);
    return ptr;
  }
 /**
  * Decodes a byte string to replace the properties of this Frame.
  *
  * A Controller object must be instantiated for this function to succeed, but
  * it does not need to be connected. To extract gestures from the deserialized
  * frame, you must enable the appropriate gestures first.
  *
  * Any existing data in the frame is
  * destroyed. If you have references to
  * child objects (hands, fingers, etc.), these are preserved as long as the
  * references remain in scope.
  *
  * \include Frame_deserialize.txt
  *
  * **Note:** The behavior when calling functions which take
  * another Frame object as a parameter is undefined when either frame has
  * been deserialized. For example, calling ``gestures(sinceFrame)`` on a
  * deserialized frame or with a deserialized frame as parameter (or both)
  * does not necessarily return all gestures that occurred between the two
  * frames. Motion functions, like ``scaleFactor(startFrame)``, are more
  * likely to return reasonable results, but could return anomalous values
  * in some cases.
  *
  * @param str A byte array containing the bytes of a serialized frame.
  * @since 2.1.0
  */
  public void deserialize(byte[] str) {
    LeapJNI.Frame_deserialize(swigCPtr, this, str, str.length);
  }
%}

%ignore Leap::DEG_TO_RAD;
%ignore Leap::RAD_TO_DEG;
%ignore Leap::PI;

// Use proper Java enums
%include "enums.swg"
%javaconst(1);

SWIG_JAVABODY_PROXY(public, public, SWIGTYPE)

#endif

// Ignore C++ streaming operator
%ignore operator<<;
// Ignore C++ equal operator
%ignore operator=;

#if SWIGPYTHON
%begin %{
#if defined(_WIN32) && defined(_DEBUG)
// Workaround for obscure STL template error
#include <vector>
// Workaround for non-existent Python debug library
#define _TMP_DEBUG _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG _TMP_DEBUG
#undef _TMP_DEBUG
#endif
#if defined(__APPLE__)
#pragma GCC diagnostic ignored "-Wself-assign"
#endif
%}
#endif

#if SWIGCSHARP || SWIGJAVA
%begin %{
#if defined(_WIN32)
#include <windows.h>
// When dynamically loading the Leap C# DLL, set the DLL search path to look in
// the same the directory. This will allow loading the Leap.dll. Create the
// Leap C# DLL with the /DELAYLOAD:Leap.dll link option.
extern "C" BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD fdwReason,
    _In_ LPVOID lpvReserved)
{
  if (lpvReserved == 0) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
      TCHAR lpPathName[1024];
      int len;

      len = static_cast<int>(GetModuleFileName(static_cast<HMODULE>(hinstDLL),
                         lpPathName, static_cast<DWORD>(sizeof(lpPathName))));
      if (len > 0 && len < sizeof(lpPathName)) {
        for (int i = len; i >= 0; i--) {
          if (lpPathName[i] == '\\' || lpPathName[i] == '/') {
            lpPathName[i] = '\0';
            static TCHAR lpPathVar[8192]; // static to avoid stacksize concerns
            DWORD sz = GetEnvironmentVariable("PATH", lpPathVar, sizeof(lpPathVar));
            if (sz == 0) {
              // %PATH% not defined
              lpPathVar[0] = '\0';
            }
            if (sz + strlen(lpPathName) + 1 < sizeof(lpPathVar)) {
              // If someone's %PATH% is 8 kB, they have bigger problems
              static TCHAR lpTmp[8192];
              strcpy(lpTmp, lpPathVar);
              strcpy(lpPathVar, lpPathName);
              lpPathVar[strlen(lpPathName)] = ';';
              strcat(lpPathVar, lpTmp);
              SetEnvironmentVariable("PATH", lpPathVar);
            }
            break;
          }
        }
      }
    }
  }
  return TRUE;
}
#endif
%}
#endif

%typemap(csin, pre="    lock($csinput) {", post="      $csinput.Dispose();\n    }") const Leap::Controller& "Controller.getCPtr($csinput)"

%header %{
#define SWIG
#include "Leap.h"
%}

%feature("director") Leap::Listener;
#if SWIGPYTHON
%feature("director:except") {
  if ($error != NULL) {
    PyErr_Print();
  }
}
#endif

%pragma(java) jniclasscode=%{
  static {
    try {
      System.loadLibrary("LeapJava");
    } catch (UnsatisfiedLinkError e) {
      System.err.println("Native code library failed to load. \n" + e);
      System.exit(1);
    }
  }
%}

////////////////////////////////////////////////////////////////////////////////
// Operator overloading                                                         #
////////////////////////////////////////////////////////////////////////////////

#if SWIGCSHARP

%ignore *::operator+=;
%ignore *::operator-=;
%ignore *::operator*=;
%ignore *::operator/=;
%ignore *::operator!=;
%ignore Leap::Vector::toFloatPointer;
%ignore Leap::Matrix::toArray3x3;
%ignore Leap::Matrix::toArray4x4;
%ignore Leap::FloatArray;

%rename(Equals) *::operator ==;
%rename(_operator_add) *::operator +;
%rename(_operator_sub) *::operator -;
%rename(_operator_mul) *::operator *;
%rename(_operator_div) *::operator /;
%rename(_operator_get) *::operator [];
%rename(_operator_to_float) *::operator const float*;
%csmethodmodifiers *::operator + "private";
%csmethodmodifiers *::operator - "private";
%csmethodmodifiers *::operator * "private";
%csmethodmodifiers *::operator / "private";
%csmethodmodifiers *::operator [] "private";
%csmethodmodifiers *::operator const float* "private";

%typemap(cscode) Leap::Vector
%{
  /** Add vectors component-wise. */
  public static Vector operator + (Vector v1, Vector v2) {
    return v1._operator_add(v2);
  }
  /** Subtract vectors component-wise. */
  public static Vector operator - (Vector v1, Vector v2) {
    return v1._operator_sub(v2);
  }
  /** Multiply vector by a scalar. */
  public static Vector operator * (Vector v1, float scalar) {
    return v1._operator_mul(scalar);
  }
  /** Multiply vector by a scalar on the left-hand side. */
  public static Vector operator * (float scalar, Vector v1) {
    return v1._operator_mul(scalar);
  }
  /** Divide vector by a scalar. */
  public static Vector operator / (Vector v1, float scalar) {
    return v1._operator_div(scalar);
  }
  /** Negate a vector. */
  public static Vector operator - (Vector v1) {
    return v1._operator_sub();
  }
  /** Convert this vector to an array of three float values: [x,y,z]. */
  public float[] ToFloatArray() {
    return new float[] {x, y, z};
  }
%}
%typemap(cscode) Leap::Matrix
%{
  /** Multiply two matrices. */
  public static Matrix operator * (Matrix m1, Matrix m2) {
    return m1._operator_mul(m2);
  }
  /** Copy this matrix to the specified array of 9 float values in row-major order. */
  public float[] ToArray3x3(float[] output) {
    output[0] = xBasis.x; output[1] = xBasis.y; output[2] = xBasis.z;
    output[3] = yBasis.x; output[4] = yBasis.y; output[5] = yBasis.z;
    output[6] = zBasis.x; output[7] = zBasis.y; output[8] = zBasis.z;
    return output;
  }
  /** Copy this matrix to the specified array containing 9 double values in row-major order. */
  public double[] ToArray3x3(double[] output) {
    output[0] = xBasis.x; output[1] = xBasis.y; output[2] = xBasis.z;
    output[3] = yBasis.x; output[4] = yBasis.y; output[5] = yBasis.z;
    output[6] = zBasis.x; output[7] = zBasis.y; output[8] = zBasis.z;
    return output;
  }
  /** Convert this matrix to an array containing 9 float values in row-major order. */
  public float[] ToArray3x3() {
    return ToArray3x3(new float[9]);
  }
  /** Copy this matrix to the specified array of 16 float values in row-major order. */
  public float[] ToArray4x4(float[] output) {
    output[0]  = xBasis.x; output[1]  = xBasis.y; output[2]  = xBasis.z; output[3]  = 0.0f;
    output[4]  = yBasis.x; output[5]  = yBasis.y; output[6]  = yBasis.z; output[7]  = 0.0f;
    output[8]  = zBasis.x; output[9]  = zBasis.y; output[10] = zBasis.z; output[11] = 0.0f;
    output[12] = origin.x; output[13] = origin.y; output[14] = origin.z; output[15] = 1.0f;
    return output;
  }
  /** Copy this matrix to the specified array of 16 double values in row-major order. */
  public double[] ToArray4x4(double[] output) {
    output[0]  = xBasis.x; output[1]  = xBasis.y; output[2]  = xBasis.z; output[3]  = 0.0f;
    output[4]  = yBasis.x; output[5]  = yBasis.y; output[6]  = yBasis.z; output[7]  = 0.0f;
    output[8]  = zBasis.x; output[9]  = zBasis.y; output[10] = zBasis.z; output[11] = 0.0f;
    output[12] = origin.x; output[13] = origin.y; output[14] = origin.z; output[15] = 1.0f;
    return output;
  }
  /** Convert this matrix to an array containing 16 float values in row-major order. */
  public float[] ToArray4x4() {
    return ToArray4x4(new float[16]);
  }
%}

#elif SWIGJAVA

%ignore *::operator+=;
%ignore *::operator-=;
%ignore *::operator*=;
%ignore *::operator/=;
%ignore *::operator!=;
%ignore Leap::Vector::toFloatPointer;
%ignore Leap::Matrix::toArray3x3;
%ignore Leap::Matrix::toArray4x4;
%ignore Leap::FloatArray;

%rename(plus) *::operator+;
%rename(minus) *::operator-;
%rename(opposite) *::operator-() const;
%rename(times) *::operator*;
%rename(divide) *::operator/;
%rename(get) *::operator [];
%rename(equals) *::operator==;

%typemap(javacode) Leap::Vector
%{
  public float[] toFloatArray() {
    return new float[] {getX(), getY(), getZ()};
  }
%}
%typemap(javacode) Leap::Matrix
%{
  public float[] toArray3x3(float[] output) {
    output[0] = getXBasis().getX(); output[1] = getXBasis().getY(); output[2] = getXBasis().getZ();
    output[3] = getYBasis().getX(); output[4] = getYBasis().getY(); output[5] = getYBasis().getZ();
    output[6] = getZBasis().getX(); output[7] = getZBasis().getY(); output[8] = getZBasis().getZ();
    return output;
  }
  public double[] toArray3x3(double[] output) {
    output[0] = getXBasis().getX(); output[1] = getXBasis().getY(); output[2] = getXBasis().getZ();
    output[3] = getYBasis().getX(); output[4] = getYBasis().getY(); output[5] = getYBasis().getZ();
    output[6] = getZBasis().getX(); output[7] = getZBasis().getY(); output[8] = getZBasis().getZ();
    return output;
  }
  public float[] toArray3x3() {
    return toArray3x3(new float[9]);
  }
  public float[] toArray4x4(float[] output) {
    output[0]  = getXBasis().getX(); output[1]  = getXBasis().getY(); output[2]  = getXBasis().getZ(); output[3]  = 0.0f;
    output[4]  = getYBasis().getX(); output[5]  = getYBasis().getY(); output[6]  = getYBasis().getZ(); output[7]  = 0.0f;
    output[8]  = getZBasis().getX(); output[9]  = getZBasis().getY(); output[10] = getZBasis().getZ(); output[11] = 0.0f;
    output[12] = getOrigin().getX(); output[13] = getOrigin().getY(); output[14] = getOrigin().getZ(); output[15] = 1.0f;
    return output;
  }
  public double[] toArray4x4(double[] output) {
    output[0]  = getXBasis().getX(); output[1]  = getXBasis().getY(); output[2]  = getXBasis().getZ(); output[3]  = 0.0f;
    output[4]  = getYBasis().getX(); output[5]  = getYBasis().getY(); output[6]  = getYBasis().getZ(); output[7]  = 0.0f;
    output[8]  = getZBasis().getX(); output[9]  = getZBasis().getY(); output[10] = getZBasis().getZ(); output[11] = 0.0f;
    output[12] = getOrigin().getX(); output[13] = getOrigin().getY(); output[14] = getOrigin().getZ(); output[15] = 1.0f;
    return output;
  }
  public float[] toArray4x4() {
    return toArray4x4(new float[16]);
  }
%}

#elif SWIGPYTHON

%ignore Leap::Interface::operator=;
%ignore Leap::ConstListIterator::operator++;
%ignore Leap::Vector::toFloatPointer;
%ignore Leap::Matrix::toArray3x3;
%ignore Leap::Matrix::toArray4x4;
%ignore Leap::FloatArray;

%rename(__getitem__) *::operator [];

%extend Leap::Vector {
%pythoncode {
  def to_float_array(self): return [self.x, self.y, self.z]
  def to_tuple(self): return (self.x, self.y, self.z)
}}
%extend Leap::Matrix {
%pythoncode {
  def to_array_3x3(self, output = None):
      if output is None:
          output = [0]*9
      output[0], output[1], output[2] = self.x_basis.x, self.x_basis.y, self.x_basis.z
      output[3], output[4], output[5] = self.y_basis.x, self.y_basis.y, self.y_basis.z
      output[6], output[7], output[8] = self.z_basis.x, self.z_basis.y, self.z_basis.z
      return output
  def to_array_4x4(self, output = None):
      if output is None:
          output = [0]*16
      output[0],  output[1],  output[2],  output[3]  = self.x_basis.x, self.x_basis.y, self.x_basis.z, 0.0
      output[4],  output[5],  output[6],  output[7]  = self.y_basis.x, self.y_basis.y, self.y_basis.z, 0.0
      output[8],  output[9],  output[10], output[11] = self.z_basis.x, self.z_basis.y, self.z_basis.z, 0.0
      output[12], output[13], output[14], output[15] = self.origin.x,  self.origin.y,  self.origin.z,  1.0
      return output
}}

#endif

////////////////////////////////////////////////////////////////////////////////
// List Helpers
////////////////////////////////////////////////////////////////////////////////

#if SWIGCSHARP

%define %leap_iterator_helper(BaseType)
%typemap(csinterfaces_derived) Leap::BaseType##List "System.Collections.Generic.IEnumerable<BaseType>"
%typemap(cscode) Leap::BaseType##List
%{
  private class BaseType##List##Enumerator : System.Collections.Generic.IEnumerator<BaseType> {
    private BaseType##List _list;
    private int _index;
    public BaseType##List##Enumerator(BaseType##List list) {
      _list = list;
      _index = -1;
    }
    public void Reset() {
      _index = -1;
    }
    public BaseType Current {
      get {
        return _list._operator_get(_index);
      }
    }
    object System.Collections.IEnumerator.Current {
      get {
        return this.Current;
      }
    }
    public bool MoveNext() {
      _index++;
      return (_index < _list.Count);
    }
    public void Dispose() {
      //No cleanup needed
    }
  }
  System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator() {
    return this.GetEnumerator();
  }
  public System.Collections.Generic.IEnumerator<BaseType> GetEnumerator() {
    return new BaseType##List##Enumerator(this);
  }
  public BaseType this[int index] {
    get { return _operator_get(index); }
  }
%}
%enddef

#elif SWIGJAVA

%define %leap_iterator_helper(BaseType)
%typemap(javainterfaces) Leap::BaseType##List "Iterable<BaseType>"
%typemap(javacode) Leap::BaseType##List
%{
  public class BaseType##ListIterator implements java.util.Iterator<BaseType> {
    int index = 0;
    @Override public boolean hasNext() {
      return index < count();
    }
    @Override public BaseType next() {
      return get(index++);
    }
    @Override public void remove() {
    }
  }
  @Override public java.util.Iterator<BaseType> iterator() {
    return new BaseType##ListIterator();
  }
%}
%enddef

#elif SWIGPYTHON

%define %leap_iterator_helper(BaseType)
%rename(__len__) Leap::BaseType##List::count;
%extend Leap::BaseType##List {
%pythoncode {
  def __iter__(self):
    _pos = 0
    while _pos < len(self):
      yield self[_pos]
      _pos += 1
}}
%enddef

#else

%define %leap_iterator_helper(BaseType)
%enddef

#endif

%define %leap_list_helper(BaseType)
%ignore Leap::BaseType##List::BaseType##List(const ListBaseImplementation<BaseType>&);
%ignore Leap::BaseType##List::const_iterator;
%ignore Leap::BaseType##List::begin() const;
%ignore Leap::BaseType##List::end() const;
%leap_iterator_helper(BaseType)
%enddef

%leap_list_helper(Pointable);
%leap_list_helper(Finger);
%leap_list_helper(Tool);
%leap_list_helper(Gesture);
%leap_list_helper(Image);
%leap_list_helper(Hand);
%leap_list_helper(Device);
%leap_list_helper(FailedDevice);

////////////////////////////////////////////////////////////////////////////////
// Config Helpers
////////////////////////////////////////////////////////////////////////////////

#if SWIGPYTHON

// Use dynamic typing to get or set any type of config value with one function
%extend Leap::Config {
%pythoncode {
  def get(self, *args):
    type = LeapPython.Config_type(self, *args)
    if type == LeapPython.Config_TYPE_BOOLEAN:
      return LeapPython.Config_get_bool(self, *args)
    elif type == LeapPython.Config_TYPE_INT32:
      return LeapPython.Config_get_int_32(self, *args)
    elif type == LeapPython.Config_TYPE_FLOAT:
      return LeapPython.Config_get_float(self, *args)
    elif type == LeapPython.Config_TYPE_STRING:
      return LeapPython.Config_get_string(self, *args)
    return None
  def set(self, *args):
    type = LeapPython.Config_type(self, *args[:-1])  # Do not pass value through
    if type == LeapPython.Config_TYPE_BOOLEAN:
      return LeapPython.Config_set_bool(self, *args)
    elif type == LeapPython.Config_TYPE_INT32:
      return LeapPython.Config_set_int_32(self, *args)
    elif type == LeapPython.Config_TYPE_FLOAT:
      return LeapPython.Config_set_float(self, *args)
    elif type == LeapPython.Config_TYPE_STRING:
      return LeapPython.Config_set_string(self, *args)
    return False
}}
// Ignore methods that are unnecessary due to get and set functions defined above
%feature("shadow") Leap::Config::type(const std::string& key) const %{%}
%feature("shadow") Leap::Config::getBool(const std::string& key) const %{%}
%feature("shadow") Leap::Config::setBool(const std::string& key, bool value) %{%}
%feature("shadow") Leap::Config::getInt32(const std::string& key) const %{%}
%feature("shadow") Leap::Config::setInt32(const std::string& key, int32_t value) %{%}
%feature("shadow") Leap::Config::getFloat(const std::string& key) const %{%}
%feature("shadow") Leap::Config::setFloat(const std::string& key, float value) %{%}
%feature("shadow") Leap::Config::getString(const std::string& key) const %{%}
%feature("shadow") Leap::Config::setString(const std::string& key, const std::string& value) %{%}

#endif

////////////////////////////////////////////////////////////////////////////////
// ToString methods
////////////////////////////////////////////////////////////////////////////////

#if SWIGCSHARP

%csmethodmodifiers *::toString "public override";

#elif SWIGJAVA

%javamethodmodifiers *::toString "@Override public";

#elif SWIGPYTHON

%rename(__str__) *::toString;

#endif



%include "LeapMath.h"
%include "Leap.h"
