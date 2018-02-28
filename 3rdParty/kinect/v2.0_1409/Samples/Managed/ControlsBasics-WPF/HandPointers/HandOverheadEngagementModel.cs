//------------------------------------------------------------------------------
// <copyright file="HandOverheadEngagementModel.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.Samples.Kinect.ControlsBasics
{
    using System;
    using System.Collections.Generic;
    using Microsoft.Kinect;
    using Microsoft.Kinect.Input;
    using Microsoft.Kinect.Toolkit.Input;
    using System.Diagnostics;

    /// <summary>
    /// A kinect engagement handler which will engage 0-2 people.
    /// Engagement signal: putting a hand over the head
    /// Disengagement signal: putting you hand down to your side
    /// </summary>
    public class HandOverheadEngagementModel : IKinectEngagementManager
    {
        bool stopped = true;
        BodyFrameReader bodyReader;
        Body[] bodies;
        bool engagementPeopleHaveChanged;
        List<BodyHandPair> handsToEngage;
        private int engagedPeopleAllowed;

        public HandOverheadEngagementModel(int engagedPeopleAllowed)
        {
            this.EngagedPeopleAllowed = engagedPeopleAllowed;
            var sensor = KinectSensor.GetDefault();
            this.bodyReader = sensor.BodyFrameSource.OpenReader();
            this.bodyReader.FrameArrived += this.BodyReader_FrameArrived;
            sensor.Open();
            this.bodies = new Body[this.bodyReader.BodyFrameSource.BodyCount];
            this.handsToEngage = new List<BodyHandPair>();
        }

        public int EngagedPeopleAllowed
        {
            get
            {
                return this.engagedPeopleAllowed;
            }
            set
            {
                if (value > 2 || value < 0)
                {
                    throw new ArgumentOutOfRangeException("value", value, "This engagement manager requires 0 to 2 people to be set as the EngagedPeopleAllowed");
                }

                this.engagedPeopleAllowed = value;
            }
        }

        public bool EngagedBodyHandPairsChanged()
        {
            return this.engagementPeopleHaveChanged;
        }

        public IReadOnlyList<BodyHandPair> KinectManualEngagedHands
        {
            get
            {
                return KinectCoreWindow.KinectManualEngagedHands;
            }
        }

        public void StartManaging()
        {
            this.stopped = false;
            this.bodyReader.IsPaused = false;
        }

        public void StopManaging()
        {
            this.stopped = true;
            this.bodyReader.IsPaused = true;
        }

        private void BodyReader_FrameArrived(object sender, BodyFrameArrivedEventArgs args)
        {
            bool gotData = false;

            using (var frame = args.FrameReference.AcquireFrame())
            {
                if (frame != null)
                {
                    frame.GetAndRefreshBodyData(bodies);
                    gotData = true;
                }
            }

            if (gotData && !stopped)
            {
                this.TrackEngagedPlayersViaHandOverHead();
            }
        }

        private static bool IsHandOverhead(JointType jointType, Body body)
        {
            return (body.Joints[jointType].Position.Y >
                    body.Joints[JointType.Head].Position.Y);
        }

        private static bool IsHandBelowHip(JointType jointType, Body body)
        {
            return (body.Joints[jointType].Position.Y <
                    body.Joints[JointType.SpineBase].Position.Y);
        }

        private void TrackEngagedPlayersViaHandOverHead()
        {
            this.engagementPeopleHaveChanged = false;
            var currentlyEngagedHands = KinectCoreWindow.KinectManualEngagedHands;
            this.handsToEngage.Clear();

            // check to see if anybody who is currently engaged should be disengaged
            foreach (var bodyHandPair in currentlyEngagedHands)
            {
                var bodyTrackingId = bodyHandPair.BodyTrackingId;
                foreach (var body in this.bodies)
                {
                    if (body.TrackingId == bodyTrackingId)
                    {
                        // check for disengagement
                        JointType engagedHandJoint =
                            (bodyHandPair.HandType == HandType.LEFT) ? JointType.HandLeft : JointType.HandRight;
                        bool toBeDisengaged = HandOverheadEngagementModel.IsHandBelowHip(engagedHandJoint, body);

                        if (toBeDisengaged)
                        {
                            this.engagementPeopleHaveChanged = true;
                        }
                        else
                        {
                            this.handsToEngage.Add(bodyHandPair);
                        }
                    }
                }
            }

            // check to see if anybody should be engaged, if not already engaged
            foreach (var body in this.bodies)
            {
                if (this.handsToEngage.Count < this.engagedPeopleAllowed)
                {
                    bool alreadyEngaged = false;
                    foreach (var bodyHandPair in this.handsToEngage)
                    {
                        alreadyEngaged = (body.TrackingId == bodyHandPair.BodyTrackingId);
                    }

                    if (!alreadyEngaged)
                    {
                        // check for engagement
                        if (HandOverheadEngagementModel.IsHandOverhead(JointType.HandLeft, body))
                        {
                            // engage the left hand
                            this.handsToEngage.Add(
                                new BodyHandPair(body.TrackingId, HandType.LEFT));
                            this.engagementPeopleHaveChanged = true;
                        }
                        else if (HandOverheadEngagementModel.IsHandOverhead(JointType.HandRight, body))
                        {
                            // engage the right hand
                            this.handsToEngage.Add(
                                new BodyHandPair(body.TrackingId, HandType.RIGHT));
                            this.engagementPeopleHaveChanged = true;
                        }
                    }
                }
            }

            if (this.engagementPeopleHaveChanged)
            {
                BodyHandPair firstPersonToEngage = null;
                BodyHandPair secondPersonToEngage = null;

                Debug.Assert(this.handsToEngage.Count <= 2, "handsToEngage should be <= 2");

                switch (this.handsToEngage.Count)
                {
                    case 0:
                        break;
                    case 1:
                        firstPersonToEngage = this.handsToEngage[0];
                        break;
                    case 2:
                        firstPersonToEngage = this.handsToEngage[0];
                        secondPersonToEngage = this.handsToEngage[1];
                        break;
                }

                switch (this.EngagedPeopleAllowed)
                {
                    case 1:
                        KinectCoreWindow.SetKinectOnePersonManualEngagement(firstPersonToEngage);
                        break;
                    case 2:
                        KinectCoreWindow.SetKinectTwoPersonManualEngagement(firstPersonToEngage, secondPersonToEngage);
                        break;
                }
            }
        }
    }
}
