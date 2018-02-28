
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
   * The Frame class represents a set of hand and finger tracking data detected
   * in a single frame.
   *
   * The Leap Motion software detects hands, fingers and tools within the tracking area, reporting
   * their positions, orientations, gestures, and motions in frames at the Leap Motion frame rate.
   *
   * Access Frame objects through an instance of the Controller class:
   *
   * \include Controller_Frame_1.txt
   *
   * Implement a Listener subclass to receive a callback event when a new Frame is available.
   * @since 1.0
   */
  [Serializable]
  public class Frame: IEquatable<Frame>
  {
    [ThreadStatic]
    private static Queue<Hand> _handPool;

    /**
     * Constructs a Frame object.
     *
     * Frame instances created with this constructor are invalid.
     * Get valid Frame objects by calling the Controller::frame() function.
     *
     * \include Frame_Frame.txt
     *
     * The only time you should use this constructor is before deserializing
     * serialized frame data. Call ``Frame::deserialize(string)`` to recreate
     * a saved Frame.
     *
     * @since 1.0
     */
    public Frame() {
      Hands = new List<Hand> ();
    }

    /**
     * Constructs a new Frame.
     *
     * @param id The id of this frame.
     * @param timestamp The creation time of this frame in microseconds.
     * @param fps The current data frame rate of the service when this frame was created.
     * @param interactionBox The InteractionBox object for this frame.
     * @since 3.0
     */
    public Frame(long id, long timestamp, float fps, InteractionBox interactionBox, List<Hand> hands)
    {
      Id = id;
      Timestamp = timestamp;
      CurrentFramesPerSecond = fps;
      InteractionBox = interactionBox;
      Hands = hands;
    }

    /**
     * Encodes this Frame object as a byte string.
     *
     * \include Frame_serialize.txt
     *
     * @since 2.1.0
     */
    public byte[] Serialize
    {
      get
      {
        byte[] ptr = new byte[1];
        ptr[1] = 0;
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
    public void Deserialize(byte[] arg)
    {

    }

    /**
     * The Hand object with the specified ID in this frame.
     *
     * Use the Frame::hand() function to retrieve the Hand object from
     * this frame using an ID value obtained from a previous frame.
     * This function always returns a Hand object, but if no hand
     * with the specified ID is present, an invalid Hand object is returned.
     *
     * \include Frame_hand.txt
     *
     * Note that ID values persist across frames, but only until tracking of a
     * particular object is lost. If tracking of a hand is lost and subsequently
     * regained, the new Hand object representing that physical hand may have
     * a different ID than that representing the physical hand in an earlier frame.
     *
     * @param id The ID value of a Hand object from a previous frame.
     * @returns The Hand object with the matching ID if one exists in this frame;
     * otherwise, an invalid Hand object is returned.
     * @since 1.0
     */
    public Hand Hand(int id)
    {
      for (int i = Hands.Count; i-- != 0; )
      {
        if (Hands[i].Id == id)
        {
          return Hands[i];
        }
      }
      return null;
    }

    /**
     * Compare Frame object equality.
     *
     * \include Frame_operator_equals.txt
     *
     * Two Frame objects are equal if and only if both Frame objects represent
     * the exact same frame of tracking data and both Frame objects are valid.
     * @since 1.0
     */
    public bool Equals(Frame other)
    {
      return this.Id == other.Id && this.Timestamp == other.Timestamp;
    }

    /**
     * A string containing a brief, human readable description of the Frame object.
     *
     * @returns A description of the Frame as a string.
     * @since 1.0
     */
    public override string ToString()
    {
      return "Frame id: " + this.Id + " timestamp: " + this.Timestamp;
    }

    /**
     * A unique ID for this Frame.
     *
     * Consecutive frames processed by the Leap Motion software have consecutive
     * increasing values. You can use the frame ID to avoid processing the same
     * Frame object twice:
     *
     * \include Frame_Duplicate.txt
     *
     * As well as to make sure that your application processes every frame:
     *
     * \include Frame_Skipped.txt
     *
     * @returns The frame ID.
     * @since 1.0
     */
    public long Id;

    /**
     * The frame capture time in microseconds elapsed since an arbitrary point in
     * time in the past.
     *
     * Use Controller::now() to calculate the age of the frame.
     *
     * \include Frame_timestamp.txt
     *
     * @returns The timestamp in microseconds.
     * @since 1.0
     */
    public long Timestamp;

    /**
     * The instantaneous framerate.
     *
     * The rate at which the Leap Motion software is providing frames of data
     * (in frames per second). The framerate can fluctuate depending on available computing
     * resources, activity within the device field of view, software tracking settings,
     * and other factors.
     *
     * \include Frame_currentFramesPerSecond.txt
     *
     * @returns An estimate of frames per second of the Leap Motion Controller.
     * @since 1.0
     */
    public float CurrentFramesPerSecond;

    /**
     * The list of Hand objects detected in this frame, given in arbitrary order.
     * The list can be empty if no hands are detected.
     *
     * \include Frame_hands.txt
     *
     * @returns The List<Hand> containing all Hand objects detected in this frame.
     * @since 1.0
     */
    public List<Hand> Hands;

    /**
     * The current InteractionBox for the frame. See the InteractionBox class
     * documentation for more details on how this class should be used.
     *
     * \include Frame_interactionBox.txt
     *
     * @returns The current InteractionBox object.
     * @since 1.0
     */
    public InteractionBox InteractionBox;

    public int SerializeLength
    {
      get { return 0; }
    }

    /**
     * Resizes the Hand list to have a specific size.  If the size is decreased,
     * the removed hands are placed into the hand pool.  If the size is increased, the
     * new spaces are filled with hands taken from the hand pool.  If the pool is
     * empty, new hands are constructed instead.
     */
    internal void ResizeHandList(int count)
    {
      if (_handPool == null)
      {
        _handPool = new Queue<Hand>();
      }

      while (Hands.Count < count)
      {
        Hand newHand;
        if (_handPool.Count > 0)
        {
          newHand = _handPool.Dequeue();
        } else {
          newHand = new Hand();
        }
        Hands.Add(newHand);
      }

      while (Hands.Count > count)
      {
        Hand lastHand = Hands[Hands.Count - 1];
        Hands.RemoveAt(Hands.Count - 1);
        _handPool.Enqueue(lastHand);
      }
    }
  }
}
