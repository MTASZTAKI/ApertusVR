//------------------------------------------------------------------------------
// <copyright file="ApplicationView.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "pch.h"
#include "ApplicationView.h"

using namespace Windows::Foundation;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;

//--------------------------------------------------------------------------------------
// Name: ApplicationView()
// Desc: Constructor
//--------------------------------------------------------------------------------------
ApplicationView::ApplicationView()
{
    m_activationComplete = false;
    m_windowClosed = false;
}

//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: Called by the system.  Perform application initialization here,
//       hooking application wide events, etc.
//--------------------------------------------------------------------------------------
void ApplicationView::Initialize(CoreApplicationView^ applicationView)
{
    applicationView->Activated += ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &ApplicationView::OnActivated);

    CoreApplication::Suspending += ref new EventHandler<SuspendingEventArgs^>(this, &ApplicationView::OnSuspending);

    CoreApplication::Resuming += ref new EventHandler<Platform::Object^>(this, &ApplicationView::OnResuming);
}

//--------------------------------------------------------------------------------------
// Name: SetWindow()
// Desc: Called when we are provided a window.
//--------------------------------------------------------------------------------------
void ApplicationView::SetWindow(CoreWindow^ window)
{
    window->Closed += ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &ApplicationView::OnWindowClosed);
}

//--------------------------------------------------------------------------------------
// Name: Load()
// Desc: The purpose of this method is to get the application entry point.
//--------------------------------------------------------------------------------------
void ApplicationView::Load(Platform::String^ entryPoint)
{
    UNREFERENCED_PARAMETER(entryPoint);
}

//--------------------------------------------------------------------------------------
// Name: Run()
// Desc: Called by the system after initialization is complete.  This
//       implements the traditional game loop
//--------------------------------------------------------------------------------------
void ApplicationView::Run()
{
    m_game = ref new ControlsBasicsDX();
    m_game->Initialize(CoreWindow::GetForCurrentThread());
    
    // Ensure we finish activation and let the system know we’re responsive
    while (!m_activationComplete)
    {
        CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
    }

    m_game->LoadContent();
    
    while (!m_windowClosed)
    {
        CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

        m_game->Tick();
    }
}

//--------------------------------------------------------------------------------------
// Name: Uninitialize()
// Desc: Called by the system.  Perform application deinitialization here.
//--------------------------------------------------------------------------------------
void ApplicationView::Uninitialize()
{
}

//--------------------------------------------------------------------------------------
// Name: OnActivated()
// Desc: Called when the application is activated.  For now, there is just one activation
//       Kind - Launch.
//--------------------------------------------------------------------------------------
void ApplicationView::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
    UNREFERENCED_PARAMETER(applicationView);
    UNREFERENCED_PARAMETER(args);

    CoreWindow::GetForCurrentThread()->Activate();
    m_activationComplete = true;
}

//--------------------------------------------------------------------------------------
// Name: OnSuspending()
// Desc: Called when the application is suspending.
//--------------------------------------------------------------------------------------
void ApplicationView::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
    UNREFERENCED_PARAMETER(sender);
    UNREFERENCED_PARAMETER(args);
}

//--------------------------------------------------------------------------------------
// Name: OnResuming()
// Desc: Called when the application is resuming from suspended.
//--------------------------------------------------------------------------------------
void ApplicationView::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
    UNREFERENCED_PARAMETER(sender);
    UNREFERENCED_PARAMETER(args);
}

//--------------------------------------------------------------------------------------
// Name: OnWindowClosed()
// Desc: Handle window closing
//--------------------------------------------------------------------------------------
void ApplicationView::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
    UNREFERENCED_PARAMETER(sender);
    UNREFERENCED_PARAMETER(args);

    m_windowClosed = true;
}

//--------------------------------------------------------------------------------------
// Name: CreateView()
// Desc: Implements a IFrameworkView factory.
//--------------------------------------------------------------------------------------
IFrameworkView^ ApplicationViewSource::CreateView()
{
    return ref new ApplicationView();
}

//--------------------------------------------------------------------------------------
// Name: main()
// Desc: Application entry point
//--------------------------------------------------------------------------------------
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
    auto applicationViewSource = ref new ApplicationViewSource();

    CoreApplication::Run(applicationViewSource);

    return 0;
}
