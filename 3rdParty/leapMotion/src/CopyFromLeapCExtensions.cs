/******************************************************************************\
* Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/
namespace LeapInternal
{
  using Leap;

  public static class CopyFromLeapCExtensions
  {

    /**
     * Copies the data from an internal tracking message into a frame.
     *
     * @param trackingMsg The internal tracking message with the data to be copied into this frame.
     */
    public static Frame CopyFrom(this Frame frame, ref LEAP_TRACKING_EVENT trackingMsg)
    {
      frame.Id = (long)trackingMsg.info.frame_id;
      frame.Timestamp = (long)trackingMsg.info.timestamp;
      frame.CurrentFramesPerSecond = trackingMsg.framerate;
      frame.InteractionBox = new InteractionBox(trackingMsg.interaction_box_center.ToLeapVector(),
                                                trackingMsg.interaction_box_size.ToLeapVector());

      frame.ResizeHandList((int)trackingMsg.nHands);

      for (int i = frame.Hands.Count; i-- != 0; )
      {
        LEAP_HAND hand;
        StructMarshal<LEAP_HAND>.ArrayElementToStruct(trackingMsg.pHands, i, out hand);
        frame.Hands[i].CopyFrom(ref hand, frame.Id);
      }

      return frame;
    }

    /**
     * Copies the data from an internal hand definition into a hand.
     *
     * @param leapHand The internal hand definition to be copied into this hand.
     * @param frameId The frame id of the frame this hand belongs to.
     */
    public static Hand CopyFrom(this Hand hand, ref LEAP_HAND leapHand, long frameId)
    {
      hand.FrameId = frameId;
      hand.Id = (int)leapHand.id;

      hand.Arm.CopyFrom(leapHand.arm, Bone.BoneType.TYPE_INVALID);

      hand.Confidence = leapHand.confidence;
      hand.GrabStrength = leapHand.grab_strength;
      hand.GrabAngle = leapHand.grab_angle;
      hand.PinchStrength = leapHand.pinch_strength;
      hand.PinchDistance = leapHand.pinch_distance;
      hand.PalmWidth = leapHand.palm.width;
      hand.IsLeft = leapHand.type == eLeapHandType.eLeapHandType_Left;
      hand.TimeVisible = leapHand.visible_time;
      hand.PalmPosition = leapHand.palm.position.ToLeapVector();
      hand.StabilizedPalmPosition = leapHand.palm.stabilized_position.ToLeapVector();
      hand.PalmVelocity = leapHand.palm.velocity.ToLeapVector();
      hand.PalmNormal = leapHand.palm.normal.ToLeapVector();
      hand.Rotation = leapHand.palm.orientation.ToLeapQuaternion();
      hand.Direction = leapHand.palm.direction.ToLeapVector();
      hand.WristPosition = hand.Arm.NextJoint;

      hand.Fingers[0].CopyFrom(leapHand.thumb, Leap.Finger.FingerType.TYPE_THUMB, hand.Id, hand.TimeVisible);
      hand.Fingers[1].CopyFrom(leapHand.index, Leap.Finger.FingerType.TYPE_INDEX, hand.Id, hand.TimeVisible);
      hand.Fingers[2].CopyFrom(leapHand.middle, Leap.Finger.FingerType.TYPE_MIDDLE, hand.Id, hand.TimeVisible);
      hand.Fingers[3].CopyFrom(leapHand.ring, Leap.Finger.FingerType.TYPE_RING, hand.Id, hand.TimeVisible);
      hand.Fingers[4].CopyFrom(leapHand.pinky, Leap.Finger.FingerType.TYPE_PINKY, hand.Id, hand.TimeVisible);

      return hand;
    }

    /**
     * Copies the data from an internal finger definition into a finger.
     *
     * @param leapBone The internal finger definition to be copied into this finger.
     * @param type The finger type of this finger.
     * @param frameId The frame id of the frame this finger belongs to.
     * @param handId The hand id of the hand this finger belongs to.
     * @param timeVisible The time in seconds that this finger has been visible.
     */
    public static Finger CopyFrom(this Finger finger, LEAP_DIGIT leapBone, Finger.FingerType type, int handId, float timeVisible)
    {
      finger.Id = (handId * 10) + leapBone.finger_id;
      finger.HandId = handId;
      finger.TimeVisible = timeVisible;

      Bone metacarpal = finger.bones[0];
      Bone proximal = finger.bones[1];
      Bone intermediate = finger.bones[2];
      Bone distal = finger.bones[3];

      metacarpal.CopyFrom(leapBone.metacarpal, Leap.Bone.BoneType.TYPE_METACARPAL);
      proximal.CopyFrom(leapBone.proximal, Leap.Bone.BoneType.TYPE_PROXIMAL);
      intermediate.CopyFrom(leapBone.intermediate, Leap.Bone.BoneType.TYPE_INTERMEDIATE);
      distal.CopyFrom(leapBone.distal, Leap.Bone.BoneType.TYPE_DISTAL);

      finger.TipPosition = distal.NextJoint;
      finger.TipVelocity = leapBone.tip_velocity.ToLeapVector();
      finger.Direction = intermediate.Direction;
      finger.StabilizedTipPosition = leapBone.stabilized_tip_position.ToLeapVector();
      finger.Width = intermediate.Width;
      finger.Length = proximal.Length + intermediate.Length + (distal.Length * 0.77f); //0.77 is used in platform code for this calculation
      finger.IsExtended = leapBone.is_extended != 0;
      finger.Type = type;

      return finger;
    }

    /**
     * Copies the data from an internal bone definition into a bone.
     *
     * @param leapBone The internal bone definition to be copied into this bone.
     * @param type The bone type of this bone.
     */
    public static Bone CopyFrom(this Bone bone, LEAP_BONE leapBone, Bone.BoneType type)
    {
      bone.Type = type;
      bone.PrevJoint = leapBone.prev_joint.ToLeapVector();
      bone.NextJoint = leapBone.next_joint.ToLeapVector();
      bone.Direction = (bone.NextJoint - bone.PrevJoint);
      bone.Length = bone.Direction.Magnitude;

      if (bone.Length < float.Epsilon)
      {
        bone.Direction = Vector.Zero;
      } else {
        bone.Direction /= bone.Length;
      }

      bone.Center = (bone.PrevJoint + bone.NextJoint) / 2.0f;
      bone.Rotation = leapBone.rotation.ToLeapQuaternion();
      bone.Width = leapBone.width;

      return bone;
    }
  }
}
