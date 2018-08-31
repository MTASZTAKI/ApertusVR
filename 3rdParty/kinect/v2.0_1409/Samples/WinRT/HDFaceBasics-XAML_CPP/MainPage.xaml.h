//------------------------------------------------------------------------------
// <copyright file="MainPage.xaml.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------


#pragma once

#include "MainPage.g.h"

using namespace WindowsPreview::Kinect;
using namespace Microsoft::Kinect::Face;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Platform;

namespace Microsoft
{
	namespace Samples
	{
		namespace Kinect
		{
			namespace HDFaceBasics
			{
				/// <summary>
				/// An empty page that can be used on its own or navigated to within a Frame.
				/// </summary>
				[Windows::UI::Xaml::Data::Bindable]
				public ref class MainPage sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
				{
				public:

					/// <summary>
					/// Initializes a new instance of the MainPage class.
					/// </summary>
					MainPage();

					/// <summary>
					/// Property changed event
					/// </summary>
					virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

					/// <summary>
					/// Gets or sets the current status text to display
					/// </summary>
					property Platform::String^ StatusText
					{
						Platform::String^ get();
						void set(Platform::String^ value);
					}

				protected:

					/// <summary>
					/// When the page is navigated to event handler
					/// </summary>
					/// <param name="e">event arguments</param>
					virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
				private:

					/// <summary>
					/// The current status text to display
					/// </summary>
					String^ statusText;

					/// <summary>
					/// Helper function to format a status message
					/// </summary>
					/// <returns>Status text</returns>
					String^ MakeStatusText();

					/// <summary>
					/// Start a face capture on clicking the button
					/// </summary>
					/// <param name="sender">object sending the event</param>
					/// <param name="e">event arguments</param>
					void StartCaptureButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

					/// <summary>
					/// Fires when Window is Loaded
					/// </summary>
					/// <param name="sender">object sending the event</param>
					/// <param name="e">event arguments</param>
					void Page_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

					/// <summary>
					/// Currently used KinectSensor
					/// </summary>
					KinectSensor^ sensor;

					/// <summary>
					/// Body frame source to get a BodyFrameReader
					/// </summary>
					BodyFrameSource^ bodySource;

					/// <summary>
					/// Body frame reader to get body frames
					/// </summary>
					BodyFrameReader^ bodyReader;

					/// <summary>
					/// HighDefinitionFaceFrameSource to get a reader and a builder from.
					/// Also to set the currently tracked user id to get High Definition Face Frames of
					/// </summary>
					HighDefinitionFaceFrameSource^ highDefinitionFaceFrameSource;

					/// <summary>
					/// HighDefinitionFaceFrameReader to read HighDefinitionFaceFrame to get FaceAlignment
					/// </summary>
					HighDefinitionFaceFrameReader^ highDefinitionFaceFrameReader;

					/// <summary>
					/// FaceAlignment is the result of tracking a face, it has face animations location and orientation
					/// </summary>
					FaceAlignment^ currentFaceAlignment;

					/// <summary>
					/// FaceModel is a result of capturing a face
					/// </summary>
					FaceModel^ currentFaceModel;

					/// <summary>
					/// Indices don't change, save them one time is enough
					/// </summary>
					IVectorView<UINT>^ cachedFaceIndices;

					/// <summary>
					/// FaceModelBuilder is used to produce a FaceModel
					/// </summary>
					FaceModelBuilder^ hdFaceBuilder;

					/// <summary>
					/// Current face model builder collection status string
					/// </summary>
                    String^ currentCollectionStatusString;

                    /// <summary>
                    /// Current face model builder capture status string
                    /// </summary>
                    String^ currentCaptureStatusString;

                    /// <summary>
                    /// Vector of the bodies acquired from a BodyFrame
                    /// </summary>
                    IVector<Body^>^ bodies;

					/// <summary>
					/// The currently tracked body
					/// </summary>
					Body^ currentTrackedBody;

					/// <summary>
					/// The currently tracked body Id
					/// </summary>
					UINT64 currentTrackingId;

					/// <summary>
					/// Face capture operation
					/// </summary>
					IAsyncOperation<FaceModelData^>^ currentModelCollectionOperation;

					/// <summary>
					/// Initialize Kinect object
					/// </summary>
					void InitializeHDFace();

					/// <summary>
					/// This event is fired when a new HDFace frame is ready for consumption
					/// </summary>
					/// <param name="sender">object sending the event</param>
					/// <param name="e">event arguments</param>
					void HDFaceReader_FrameArrived(HighDefinitionFaceFrameReader^ sender, HighDefinitionFaceFrameArrivedEventArgs^ e);

                    /// <summary>
                    /// This event is fired when the FaceModelBuilder collection status has changed
                    /// </summary>
                    /// <param name="sender">object sending the event</param>
                    /// <param name="e">event arguments</param>
                    void FaceModelBuilder_CollectionStatusChanged(FaceModelBuilder^ sender, CollectionStatusChangedEventArgs^ e);

                    /// <summary>
                    /// This event is fired when the FaceModelBuilder capture status has changed
                    /// </summary>
                    /// <param name="sender">object sending the event</param>
                    /// <param name="e">event arguments</param>
                    void FaceModelBuilder_CaptureStatusChanged( FaceModelBuilder^ sender, CaptureStatusChangedEventArgs^ e);

					/// <summary>
					/// This event fires when a BodyFrame is ready for consumption
					/// </summary>
					/// <param name="sender">object sending the event</param>
					/// <param name="e">event arguments</param>
					void BodyReader_FrameArrived(BodyFrameReader^ sender, BodyFrameArrivedEventArgs^ e);

					/// <summary>
					/// Sends the new deformed mesh to be drawn
					/// </summary>
					void UpdateFaceMesh();

					/// <summary>
					/// Start a face capture operation
					/// </summary>
					void StartCapture();

					/// <summary>
					/// Cancel the current face capture operation
					/// </summary>
					void StopFaceCapture();

					/// <summary>
					/// Build a string from the collection status bit flag combinations (simplified)
					/// </summary>
					/// <param name="status">Status value</param>
					/// <returns>Status value as text</returns>
                    String^ BuildCollectionStatusText(FaceModelBuilderCollectionStatus status);

                    /// <summary>
                    /// Gets the current capture status
                    /// </summary>
                    /// <param name="status">Status value</param>
                    /// <returns>Status value as text</returns>
                    String^ GetCaptureStatusText(FaceModelBuilderCaptureStatus status);

					/// <summary>
					/// Finds the closest body from the sensor if any
					/// </summary>
					/// <returns>Closest body, null of none</returns>
					Body^ FindClosestBody();

					/// <summary>
					/// Find if there is a body tracked with the given trackingId
					/// </summary>
					/// <param name="trackingId">The tracking Id</param>
					/// <returns>The body object, null of none</returns>
					Body^ FindBodyWithTrackingId(UINT64 trackingId);

					/// <summary>
					/// Returns the length of a vector from origin
					/// </summary>
					/// <param name="point">Point in space to find it's distance from origin</param>
					/// <returns>Distance from origin</returns>
					double VectorLength(CameraSpacePoint point);

                    /// <summary>
                    /// CollectionStatus eventing handle
                    /// </summary>
                    Windows::Foundation::EventRegistrationToken tokenCollectionStatusChanged;

                    /// <summary>
                    /// CaptureStatus eventing handle
                    /// </summary>
                    Windows::Foundation::EventRegistrationToken tokenCaptureStatusChanged;
				};
			}
		}
	}
}