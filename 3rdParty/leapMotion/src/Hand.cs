/******************************************************************************\
* Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/
namespace Leap
{
  using System;
  using System.Collections.Generic;
  using System.Runtime.InteropServices;

  /**
   * The Hand class reports the physical characteristics of a detected hand.
   *
   * Hand tracking data includes a palm position and velocity; vectors for
   * the palm normal and direction to the fingers; properties of a sphere fit
   * to the hand; and lists of the attached fingers.
   *
   * Get Hand objects from a Frame object:
   *
   * \include Hand_Get_First.txt
   *
   * Note that Hand objects can be invalid, which means that they do not contain
   * valid tracking data and do not correspond to a physical entity. Invalid Hand
   * objects can be the result of asking for a Hand object using an ID from an
   * earlier frame when no Hand objects with that ID exist in the current frame.
   * A Hand object created from the Hand constructor is also invalid.
   * Test for validity with the Hand::isValid() function.
   * @since 1.0
   */
  [Serializable]
  public class Hand
  {
    /**
     * Constructs a Hand object.
     *
     * An uninitialized hand is considered invalid.
     * Get valid Hand objects from a Frame object.
     *
     * \include Hand_Hand.txt
     *
     * @since 1.0
     */
    public Hand()
    {
      Arm = new Arm();
      Fingers = new List<Finger>(5);
      Fingers.Add(new Finger());
      Fingers.Add(new Finger());
      Fingers.Add(new Finger());
      Fingers.Add(new Finger());
      Fingers.Add(new Finger());
    }

    /**
     * Constructs a hand.
     *
     * Generally, you should not create your own Hand objects. Such objects will not
     * have valid tracking data. Get valid Hand objects from a frame
     * received from the service.
     *
     * @param frameID Id of the frame this hand belongs to
     * @param id The hand identifier
     * @param confidence The estimate of data fitness
     * @param grabStrength The grab posture rating of the hand
     * @param grabAngle The average angle of the fingers to the hand
     * @param pinchStrength The pinch posture rating of the hand
     * @param pinchDistance The distance in millimeters between the thumb and index finger tips when pinching
     * @param palmWidth the width of the hand
     * @param isLeft True, if this is a left hand
     * @param timeVisible The amount of time this hand has been visible
     * @param arm The arm object for this hand
     * @param fingers The list of fingers for this hand
     * @param palmPosition The position of the palm
     * @param stabilizedPalmPosition The time-filtered position of the palm
     * @param palmVelocity The velocity vector of the hand
     * @param Vector palmNormal The normal vector to the palm
     * @param direction The direction of the hand
     * @param wristPosition The position of the wrist
     * @since 3.0
     */
    public Hand(long frameID,
                int id,
                float confidence,
                float grabStrength,
                float grabAngle,
                float pinchStrength,
                float pinchDistance,
                float palmWidth,
                bool isLeft,
                float timeVisible,
                Arm arm,
                List<Finger> fingers,
                Vector palmPosition,
                Vector stabilizedPalmPosition,
                Vector palmVelocity,
                Vector palmNormal,
                LeapQuaternion palmOrientation,
                Vector direction,
                Vector wristPosition)
    {
      FrameId = frameID;
      Id = id;
      Confidence = confidence;
      GrabStrength = grabStrength;
      GrabAngle = grabAngle;
      PinchStrength = pinchStrength;
      PinchDistance = pinchDistance;
      PalmWidth = palmWidth;
      IsLeft = isLeft;
      TimeVisible = timeVisible;
      Arm = arm;
      Fingers = fingers;
      PalmPosition = palmPosition;
      StabilizedPalmPosition = stabilizedPalmPosition;
      PalmVelocity = palmVelocity;
      PalmNormal = palmNormal;
      Rotation = palmOrientation;
      Direction = direction;
      WristPosition = wristPosition;
    }

    /**
     * The Finger object with the specified ID attached to this hand.
     *
     * Use the Hand::finger() function to retrieve a Finger object attached to
     * this hand using an ID value obtained from a previous frame.
     * This function always returns a Finger object, but if no finger
     * with the specified ID is present, an invalid Finger object is returned.
     *
     * \include Hand_finger.txt
     *
     * Note that ID values persist across frames, but only until tracking of a
     * particular object is lost. If tracking of a finger is lost and subsequently
     * regained, the new Finger object representing that finger may have a
     * different ID than that representing the finger in an earlier frame.
     *
     * @param id The ID value of a Finger object from a previous frame.
     * @returns The Finger object with the matching ID if one exists for this
     * hand in this frame; otherwise, an invalid Finger object is returned.
     * @since 1.0
     */
    public Finger Finger(int id)
    {
      for (int i = Fingers.Count; i-- != 0; )
      {
        if (Fingers[i].Id == id)
        {
          return Fingers[i];
        }
      }
      return null;
    }

    /**
     * Compare Hand object equality.
     *
     * \include Hand_operator_equals.txt
     *
     * Two Hand objects are equal if and only if both Hand objects represent the
     * exact same physical hand in the same frame and both Hand objects are valid.
     * @since 1.0
     */
    public bool Equals(Hand other)
    {
      return Id == other.Id && FrameId == other.FrameId;
    }

    /**
     * A string containing a brief, human readable description of the Hand object.
     *
     * @returns A description of the Hand as a string.
     * @since 1.0
     */
    public override string ToString()
    {
      return string.Format(
        "Hand {0} {1}.",
        this.Id,
        this.IsLeft ? "left" : "right"
      );
    }

    public long FrameId;

    /**
     * A unique ID assigned to this Hand object, whose value remains the same
     * across consecutive frames while the tracked hand remains visible. If
     * tracking is lost (for example, when a hand is occluded by another hand
     * or when it is withdrawn from or reaches the edge of the Leap Motion Controller field of view),
     * the Leap Motion software may assign a new ID when it detects the hand in a future frame.
     *
     * Use the ID value with the Frame::hand() function to find this Hand object
     * in future frames:
     *
     * \include Hand_Get_ID.txt
     *
     * @returns The ID of this hand.
     * @since 1.0
     */
    public int Id;

    /**
     * The list of Finger objects detected in this frame that are attached to
     * this hand, given in order from thumb to pinky.  The list cannot be empty.
     *
     * Use PointableList::extended() to remove non-extended fingers from the list.
     *
     * \include Hand_Get_Fingers.txt
     *
     * @returns The List<Finger> containing all Finger objects attached to this hand.
     * @since 1.0
     */
    public List<Finger> Fingers;


    /**
     * The center position of the palm in millimeters from the Leap Motion Controller origin.
     *
     * \include Hand_palmPosition.txt
     *
     * @returns The Vector representing the coordinates of the palm position.
     * @since 1.0
     */
    public Vector PalmPosition;

    /**
     * The rate of change of the palm position in millimeters/second.
     *
     * \include Hand_palmVelocity.txt
     *
     * @returns The Vector representing the coordinates of the palm velocity.
     * @since 1.0
     */
    public Vector PalmVelocity;

    /**
     * The normal vector to the palm. If your hand is flat, this vector will
     * point downward, or "out" of the front surface of your palm.
     *
     * \image html images/Leap_Palm_Vectors.png
     *
     * The direction is expressed as a unit vector pointing in the same
     * direction as the palm normal (that is, a vector orthogonal to the palm).
     *
     * You can use the palm normal vector to compute the roll angle of the palm with
     * respect to the horizontal plane:
     *
     * \include Hand_Get_Angles.txt
     *
     * @returns The Vector normal to the plane formed by the palm.
     * @since 1.0
     */
    public Vector PalmNormal;

    /**
     * The direction from the palm position toward the fingers.
     *
     * The direction is expressed as a unit vector pointing in the same
     * direction as the directed line from the palm position to the fingers.
     *
     * You can use the palm direction vector to compute the pitch and yaw angles of the palm with
     * respect to the horizontal plane:
     *
     * \include Hand_Get_Angles.txt
     *
     * @returns The Vector pointing from the palm position toward the fingers.
     * @since 1.0
     */
    public Vector Direction;

     /**
     * The transform of the hand.
     *
     * Note, in version prior to 3.1, the Basis was a Matrix object.
     * @since 3.1
     */
    public LeapTransform Basis { get { return new LeapTransform(PalmPosition, Rotation); } }

    /**
    * The rotation of the hand as a quaternion.
    *
    * @since 3.1
    */
    public LeapQuaternion Rotation;

    /**
     * The strength of a grab hand pose.
     *
     * The strength is zero for an open hand, and blends to 1.0 when a grabbing hand
     * pose is recognized.
     *
     * \include Hand_grabStrength.txt
     *
     * @returns A float value in the [0..1] range representing the holding strength
     * of the pose.
     * @since 2.0
     */
    public float GrabStrength;

    /**
     * The angle between the fingers and the hand of a grab hand pose.
     *
     * The angle is computed by looking at the angle between the direction of the
     * 4 fingers and the direction of the hand. Thumb is not considered when
     * computing the angle.
     * The angle is 0 radian for an open hand, and reaches pi radians when the pose
     * is a tight fist.
     *
     * @returns The angle of a grab hand pose between 0 and pi radians (0 and 180 degrees).
     * @since 3.0
     */
    public float GrabAngle;

    /**
     * The holding strength of a pinch hand pose.
     *
     * The strength is zero for an open hand, and blends to 1.0 when a pinching
     * hand pose is recognized. Pinching can be done between the thumb
     * and any other finger of the same hand.
     *
     * \include Hand_pinchStrength.txt
     *
     * @returns A float value in the [0..1] range representing the holding strength
     * of the pinch pose.
     * @since 2.0
     */
    public float PinchStrength;

    /**
     * The distance between the thumb and index finger of a pinch hand pose.
     *
     * The distance is computed by looking at the shortest distance between
     * the last 2 phalanges of the thumb and those of the index finger.
     * This pinch measurement only takes thumb and index finger into account.
     *
     * \include Hand_pinchDistance.txt
     *
     * @returns The distance between the thumb and index finger of a pinch hand
     * pose in millimeters.
     * @since 3.0
     */
    public float PinchDistance;

    /**
     * The estimated width of the palm when the hand is in a flat position.
     *
     * \include Hand_palmWidth.txt
     *
     * @returns The width of the palm in millimeters
     * @since 2.0
     */
    public float PalmWidth;

    /**
     * The stabilized palm position of this Hand.
     *
     * Smoothing and stabilization is performed in order to make
     * this value more suitable for interaction with 2D content. The stabilized
     * position lags behind the palm position by a variable amount, depending
     * primarily on the speed of movement.
     *
     * \include Hand_stabilizedPalmPosition.txt
     *
     * @returns A modified palm position of this Hand object
     * with some additional smoothing and stabilization applied.
     * @since 1.0
     */
    public Vector StabilizedPalmPosition;

    /**
     * The position of the wrist of this hand.
     *
     * @returns A vector containing the coordinates of the wrist position in millimeters.
     * @since 2.0.3
     */
    public Vector WristPosition;

    /**
     * The duration of time this Hand has been visible to the Leap Motion Controller.
     *
     * \include Hand_timeVisible.txt
     *
     * @returns The duration (in seconds) that this Hand has been tracked.
     * @since 1.0
     */
    public float TimeVisible;

    /**
     * How confident we are with a given hand pose.
     *
     * The confidence level ranges between 0.0 and 1.0 inclusive.
     *
     * \include Hand_confidence.txt
     *
     * @since 2.0
     */
    public float Confidence;

    /**
     * Identifies whether this Hand is a left hand.
     *
     * \include Hand_isLeft.txt
     *
     * @returns True if the hand is identified as a left hand.
     * @since 2.0
     */
    public bool IsLeft;

    /**
     * Identifies whether this Hand is a right hand.
     *
     * \include Hand_isRight.txt
     *
     * @returns True if the hand is identified as a right hand.
     * @since 2.0
     */
    public bool IsRight { get { return !IsLeft; } }

    /**
     * The arm to which this hand is attached.
     *
     * If the arm is not completely in view, Arm attributes are estimated based on
     * the attributes of entities that are in view combined with typical human anatomy.
     *
     * \include Arm_get.txt
     *
     * @returns The Arm object for this hand.
     * @since 2.0.3
     */
    public Arm Arm;
  }
}
