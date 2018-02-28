//------------------------------------------------------------------------------
// <copyright file="MainPage.xaml.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------


#include "pch.h"
#include "MainPage.xaml.h"
#include <collection.h>
#include <limits>

using namespace Microsoft::Samples::Kinect::HDFaceBasics;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

MainPage::MainPage():
    sensor(nullptr),
    bodySource(nullptr),
    bodyReader(nullptr),
    highDefinitionFaceFrameSource(nullptr),
    highDefinitionFaceFrameReader(nullptr),
    currentFaceAlignment(nullptr),
    currentFaceModel(nullptr),
    cachedFaceIndices(nullptr),
    hdFaceBuilder(nullptr),
    bodies(nullptr),
    currentTrackedBody(nullptr),
    currentTrackingId(0),
    currentModelCollectionOperation(nullptr),
    currentCollectionStatusString(L""),
    currentCaptureStatusString(L"Ready To Start Capture"),
    statusText(L"")
{
	InitializeComponent();
	this->DataContext = this;
	this->StatusText = MakeStatusText();
}

String^ MainPage::StatusText::get()
{
	return this->statusText; 
}

void MainPage::StatusText::set(String^ value) 
{
	if (this->statusText != value) 
	{
		this->statusText = value;

		PropertyChangedEventArgs^ args = ref new PropertyChangedEventArgs("StatusText");
		this->PropertyChanged(this, args);
	}
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
void MainPage::OnNavigatedTo(NavigationEventArgs^ /* e */)
{
	this->HDFaceRenderingPanel->StartRenderLoop();
}

void Microsoft::Samples::Kinect::HDFaceBasics::MainPage::Page_Loaded(Platform::Object^ /* sender */, Windows::UI::Xaml::RoutedEventArgs^ /* e */)
{
	this->InitializeHDFace();
}

/// <summary>
/// Initialize Kinect object
/// </summary>
void Microsoft::Samples::Kinect::HDFaceBasics::MainPage::InitializeHDFace()
{
	this->sensor = KinectSensor::GetDefault();

	this->bodySource = this->sensor->BodyFrameSource;
    this->bodies = ref new Platform::Collections::Vector<Body^>(this->bodySource->BodyCount);

	this->bodyReader = this->bodySource->OpenReader();
	this->bodyReader->FrameArrived += ref new TypedEventHandler<WindowsPreview::Kinect::BodyFrameReader^, WindowsPreview::Kinect::BodyFrameArrivedEventArgs^>(this, &Microsoft::Samples::Kinect::HDFaceBasics::MainPage::BodyReader_FrameArrived);

	this->highDefinitionFaceFrameSource = ref new HighDefinitionFaceFrameSource(this->sensor);
	this->highDefinitionFaceFrameReader = this->highDefinitionFaceFrameSource->OpenReader();
	this->highDefinitionFaceFrameReader->FrameArrived += ref new TypedEventHandler<HighDefinitionFaceFrameReader^, HighDefinitionFaceFrameArrivedEventArgs^>(this, &Microsoft::Samples::Kinect::HDFaceBasics::MainPage::HDFaceReader_FrameArrived);

	this->currentFaceModel = ref new FaceModel();
	this->currentFaceAlignment = ref new FaceAlignment();
	this->cachedFaceIndices = FaceModel::TriangleIndices;

	this->UpdateFaceMesh();

	this->sensor->Open();
}

/// <summary>
/// Sends the new deformed mesh to be drawn
/// </summary>
void Microsoft::Samples::Kinect::HDFaceBasics::MainPage::UpdateFaceMesh()
{
	auto faceVertices = this->currentFaceModel->CalculateVerticesForAlignment(this->currentFaceAlignment);
	this->HDFaceRenderingPanel->UpdateMesh(faceVertices, this->cachedFaceIndices);
}

/// <summary>
/// Creates the status text message
/// </summary>
String^ Microsoft::Samples::Kinect::HDFaceBasics::MainPage::MakeStatusText()
{
    auto text = L"Capture: " + this->currentCaptureStatusString + " Collection: " + this->currentCollectionStatusString + L", Tracking ID = " + this->currentTrackingId.ToString();
	return text;
}

/// <summary>
/// This event fires when a BodyFrame is ready for consumption
/// </summary>
void Microsoft::Samples::Kinect::HDFaceBasics::MainPage::BodyReader_FrameArrived(BodyFrameReader^ sender, BodyFrameArrivedEventArgs^ e)
{
    auto frameReference = e->FrameReference;
    {
        auto frame = frameReference->AcquireFrame();

        if (frame == nullptr)
        {
            return;
        }

        // Get the latest bodies
        frame->GetAndRefreshBodyData(this->bodies);
    }

	// Do we still see the person we're tracking?
	if (this->currentTrackedBody != nullptr)
	{
		this->currentTrackedBody = this->FindBodyWithTrackingId(this->currentTrackingId);

		if (this->currentTrackedBody != nullptr)
		{
			// We still see the person we're tracking, make no change.
			return;
		}
	}

	Body^ selectedBody = this->FindClosestBody();

	if (selectedBody == nullptr)
	{
		this->currentTrackedBody = nullptr;
		this->currentTrackingId = 0;

		return;
	}

	this->currentTrackedBody = selectedBody;
	auto trackingID = selectedBody->TrackingId;;
	this->currentTrackingId = trackingID;

	this->highDefinitionFaceFrameSource->TrackingId = this->currentTrackingId;
}

/// <summary>
/// Returns the length of a vector from origin
/// </summary>
double Microsoft::Samples::Kinect::HDFaceBasics::MainPage::VectorLength(CameraSpacePoint point)
{
	auto result = pow(point.X, 2) + pow(point.Y, 2) + pow(point.Z, 2);

	result = sqrt(result);

	return result;
}

/// <summary>
/// Finds the closest body from the sensor if any
/// </summary>
Body^ Microsoft::Samples::Kinect::HDFaceBasics::MainPage::FindClosestBody()
{
	Body^ result = nullptr;

	double closestBodyDistance = 10000000.0;

	for (auto body : this->bodies)
	{
		if (body->IsTracked)
		{
			auto joints = body->Joints;

			auto currentLocation = joints->Lookup(JointType::SpineBase).Position;

			auto currentDistance = this->VectorLength(currentLocation);

			if (result == nullptr || currentDistance < closestBodyDistance)
			{
				result = body;
				closestBodyDistance = currentDistance;
			}
		}
	}

	return result;
}

/// <summary>
/// Find if there is a body tracked with the given trackingId
/// </summary>
Body^ Microsoft::Samples::Kinect::HDFaceBasics::MainPage::FindBodyWithTrackingId(UINT64 trackingId)
{
	Body^ result = nullptr;

	for (auto body : this->bodies)
	{
		if (body->IsTracked)
		{
			if (body->TrackingId == trackingId)
			{
				result = body;
				break;
			}
		}
	}

	return result;
}

/// <summary>
/// This event is fired when a new HDFace frame is ready for consumption
/// </summary>
void Microsoft::Samples::Kinect::HDFaceBasics::MainPage::HDFaceReader_FrameArrived(HighDefinitionFaceFrameReader^ sender, HighDefinitionFaceFrameArrivedEventArgs^ e)
{
    auto frameReference = e->FrameReference;

    {
        auto frame = frameReference->AcquireFrame();

        // We might miss the chance to acquire the frame; it will be null if it's missed.
        // Also ignore this frame if face tracking failed.
        if (frame == nullptr || !frame->IsFaceTracked)
        {
            return;
        }

        frame->GetAndRefreshFaceAlignmentResult(this->currentFaceAlignment);
    }

    this->UpdateFaceMesh();
    this->StatusText = this->MakeStatusText();
}

/// <summary>
/// This event is fired when the FaceModelBuilder collection status has changed
/// </summary>
void Microsoft::Samples::Kinect::HDFaceBasics::MainPage::FaceModelBuilder_CollectionStatusChanged( FaceModelBuilder^ sender, CollectionStatusChangedEventArgs^ e)
{
    FaceModelBuilderCollectionStatus newStatus = sender->CollectionStatus;      //Query the latest status

    this->currentCollectionStatusString = BuildCollectionStatusText(newStatus);
}

/// <summary>
/// This event is fired when the FaceModelBuilder capture status has changed
/// </summary>
void Microsoft::Samples::Kinect::HDFaceBasics::MainPage::FaceModelBuilder_CaptureStatusChanged( FaceModelBuilder^ sender, CaptureStatusChangedEventArgs^ e)
{
    FaceModelBuilderCaptureStatus newStatus = sender->CaptureStatus;

    this->currentCaptureStatusString = GetCaptureStatusText(newStatus);
}

/// <summary>
/// Start a face capture on clicking the button
/// </summary>
void Microsoft::Samples::Kinect::HDFaceBasics::MainPage::StartCaptureButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->StartCapture();
}

/// <summary>
/// Cancel the current face capture operation
/// </summary>
void Microsoft::Samples::Kinect::HDFaceBasics::MainPage::StopFaceCapture()
{
	if (this->currentModelCollectionOperation != nullptr)
	{
		this->currentModelCollectionOperation->Cancel();
		this->currentModelCollectionOperation = nullptr;
	}

	if (this->hdFaceBuilder != nullptr)
	{
        this->hdFaceBuilder->CollectionStatusChanged -= this->tokenCollectionStatusChanged;
        this->hdFaceBuilder->CaptureStatusChanged -= this->tokenCaptureStatusChanged;
		this->hdFaceBuilder = nullptr;
	}
}

/// <summary>
/// Start a face capture operation
/// </summary>
void Microsoft::Samples::Kinect::HDFaceBasics::MainPage::StartCapture()
{
	if (this->currentModelCollectionOperation != nullptr)
	{
		this->StopFaceCapture();
	}

	this->hdFaceBuilder = this->highDefinitionFaceFrameSource->OpenModelBuilder(FaceModelBuilderAttributes::None);
    this->tokenCollectionStatusChanged = this->hdFaceBuilder->CollectionStatusChanged += ref new TypedEventHandler<FaceModelBuilder^, CollectionStatusChangedEventArgs^>(this, &Microsoft::Samples::Kinect::HDFaceBasics::MainPage::FaceModelBuilder_CollectionStatusChanged);
    this->tokenCaptureStatusChanged = this->hdFaceBuilder->CaptureStatusChanged += ref new TypedEventHandler<FaceModelBuilder^, CaptureStatusChangedEventArgs^>(this, &Microsoft::Samples::Kinect::HDFaceBasics::MainPage::FaceModelBuilder_CaptureStatusChanged);

	this->currentModelCollectionOperation = this->hdFaceBuilder->CollectFaceDataAsync();
    this->currentModelCollectionOperation->Completed = ref new AsyncOperationCompletedHandler<FaceModelData^>(
		[this](IAsyncOperation<FaceModelData^>^ asyncOp, AsyncStatus status)
	{
		if (status == AsyncStatus::Completed)
		{
			auto modelData = asyncOp->GetResults();
			currentFaceModel = modelData->ProduceFaceModel();
			
			StopFaceCapture();
			currentCaptureStatusString = "Capture Complete";
            currentCollectionStatusString = "";
		}
		else if (status == AsyncStatus::Error)
		{
			StopFaceCapture();
            currentCaptureStatusString = "Error collecting face data!";
            currentCollectionStatusString = "";
        }
		else
		{
			StopFaceCapture();
            currentCaptureStatusString = "Collecting face data incomplete!";
            currentCollectionStatusString = "";
        }
	});
}

String^ Microsoft::Samples::Kinect::HDFaceBasics::MainPage::BuildCollectionStatusText( FaceModelBuilderCollectionStatus status )
{
    String^ res = L"";

    if (FaceModelBuilderCollectionStatus::Complete == status)
    {
        return L" Done ";
    }

    if ((int)(status & FaceModelBuilderCollectionStatus::MoreFramesNeeded) != 0)
    {
        res += L" More ";
    }

    if ((int)(status & FaceModelBuilderCollectionStatus::FrontViewFramesNeeded) != 0)
    {
        res += L" Front";
    }

    if ((int)(int)(status & FaceModelBuilderCollectionStatus::LeftViewsNeeded) != 0)
    {
        res += L" Left ";
    }

    if ((int)(status & FaceModelBuilderCollectionStatus::RightViewsNeeded) != 0)
    {
        res += L" Right";
    }

    if ((int)(status & FaceModelBuilderCollectionStatus::TiltedUpViewsNeeded) != 0)
    {
        res += L"  Up  ";
    }

    return res;
}

String^ Microsoft::Samples::Kinect::HDFaceBasics::MainPage::GetCaptureStatusText(FaceModelBuilderCaptureStatus status)
{
    switch (status)
    {
        case FaceModelBuilderCaptureStatus::FaceTooFar:
            return L"  Too Far ";
        case FaceModelBuilderCaptureStatus::FaceTooNear:
            return L" Too Near ";
        case FaceModelBuilderCaptureStatus::GoodFrameCapture:
            return L"Good Frame";
        case FaceModelBuilderCaptureStatus::LostFaceTrack:
            return L"Lost Track";
        case FaceModelBuilderCaptureStatus::MovingTooFast:
            return L" Too Fast ";
        case FaceModelBuilderCaptureStatus::OtherViewsNeeded:
            return L"More Views";
        case FaceModelBuilderCaptureStatus::SystemError:
            return L"  Error   ";
    }

    return L"";
}


