//------------------------------------------------------------------------------
// <copyright file="EngagementSettings.xaml.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.Samples.Kinect.ControlsBasics.Pages
{
    using System;
    using System.Collections.Generic;
    using Windows.UI.Xaml;
    using Windows.UI.Xaml.Controls;
    using Windows.UI.Xaml.Navigation;
    using WindowsPreview.Kinect.Input;
    using Microsoft.Samples.Kinect.ControlsBasics.HandPointers;
    using Microsoft.Kinect.Toolkit.Input;
    using Microsoft.Samples.Kinect.ControlsBasics;
    
    

    /// <summary>
    /// A basic page that provides characteristics common to most applications.
    /// </summary>
    public sealed partial class EngagementSettings : Microsoft.Samples.Kinect.ControlsBasics.Common.LayoutAwarePage
    {
        const int CursorSpriteCols = 4;
        const int CursorSpriteRows = 20;
        const int CursorWidth = 137;
        const int CursorHeight = 137;

        public EngagementSettings()
        {
            this.InitializeComponent();
            this.Loaded += EngagementSettings_Loaded;
        }

        void EngagementSettings_Loaded(object sender, RoutedEventArgs e)
        {
            // change controls to represent current engagement settings
            // during load of page.
            App app = ((App)App.Current);

            IKinectEngagementManager kinectEngagementManager = app.KinectRegion.KinectEngagementManager;
            var handInScreenEngagementModel = kinectEngagementManager as HandInScreenEngagementModel;
            var handOverHeadEngagementModel = kinectEngagementManager as HandOverheadEngagementModel;

            switch (KinectCoreWindow.KinectEngagementMode)
            {
                case KinectEngagementMode.SystemOnePerson:
                    onePerson.IsChecked = true;
                    system.IsChecked = true;
                    break;
                case KinectEngagementMode.SystemTwoPerson:
                    twoPerson.IsChecked = true;
                    system.IsChecked = true;
                    break;
                case KinectEngagementMode.ManualOnePerson:
                    onePerson.IsChecked = true;
                    if (handInScreenEngagementModel != null)
                    {
                        manualOnScreen.IsChecked = true;
                    }
                    else if (handOverHeadEngagementModel != null)
                    {
                        manualOverHead.IsChecked = true;
                    }
                    break;
                case KinectEngagementMode.ManualTwoPerson:
                    twoPerson.IsChecked = true;
                    if (handInScreenEngagementModel != null)
                    {
                        manualOnScreen.IsChecked = true;
                    }
                    else if (handOverHeadEngagementModel != null)
                    {
                        manualOverHead.IsChecked = true;
                    }
                    break;
            }

            // Manage cursor sprite sheets
            if (app.KinectRegion.CursorSpriteSheetDefinition == Microsoft.Kinect.Xaml.Controls.KinectRegion.DefaultSpriteSheet)
            {
                cursorSpriteSheetDefault.IsChecked = true;
            }
            else
            {
                cursorSpriteSheetColor.IsChecked = true;
            }
        }

        // When navigated away, we communicate with KinectRegion about desired engagement mode.
        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            int people = onePerson.IsChecked.HasValue && onePerson.IsChecked.Value ? 1 : 2;
            App app = ((App)App.Current);

            if (system.IsChecked.HasValue && system.IsChecked.Value)
            {
                switch (people)
                {
                    case 1:
                        app.KinectRegion.SetKinectOnePersonSystemEngagement();
                        break;
                    case 2:
                        app.KinectRegion.SetKinectTwoPersonSystemEngagement();
                        break;
                }
            }
            else if (manualOverHead.IsChecked.HasValue && manualOverHead.IsChecked.Value)
            {
                var engagementModel = new HandOverheadEngagementModel(people);
                switch (people)
                {
                    case 1:
                        app.KinectRegion.SetKinectOnePersonManualEngagement(engagementModel);
                        break;
                    case 2:
                        app.KinectRegion.SetKinectTwoPersonManualEngagement(engagementModel);
                        break;
                }
            }
            else if (manualOnScreen.IsChecked.HasValue && manualOnScreen.IsChecked.Value)
            {
                var engagementModel = new HandInScreenEngagementModel(people, app.KinectRegion.InputPointerManager);
                switch (people)
                {
                    case 1:
                        app.KinectRegion.SetKinectOnePersonManualEngagement(engagementModel);
                        break;
                    case 2:
                        app.KinectRegion.SetKinectTwoPersonManualEngagement(engagementModel);
                        break;
                }
            }

            // Manage cursor sprite sheets
            if (cursorSpriteSheetDefault.IsChecked.HasValue && cursorSpriteSheetDefault.IsChecked.Value)
            {
                app.KinectRegion.CursorSpriteSheetDefinition = Microsoft.Kinect.Xaml.Controls.KinectRegion.DefaultSpriteSheet;
            }
            else if (cursorSpriteSheetColor.IsChecked.HasValue && cursorSpriteSheetColor.IsChecked.Value)
            {
                app.KinectRegion.CursorSpriteSheetDefinition = new CursorSpriteSheetDefinition(
                    new System.Uri("ms-appx:///Assets/CursorSpriteSheetPurple.png"),
                    CursorSpriteCols,
		            CursorSpriteRows,
                    CursorWidth,
                    CursorHeight
                );
            }

            base.OnNavigatedFrom(e);
        }

        /// <summary>
        /// Populates the page with content passed during navigation.  Any saved state is also
        /// provided when recreating a page from a prior session.
        /// </summary>
        /// <param name="navigationParameter">The parameter value passed to
        /// <see cref="Frame.Navigate(Type, Object)"/> when this page was initially requested.
        /// </param>
        /// <param name="pageState">A dictionary of state preserved by this page during an earlier
        /// session.  This will be null the first time a page is visited.</param>
        protected override void LoadState(Object navigationParameter, Dictionary<String, Object> pageState)
        {
        }

        /// <summary>
        /// Preserves state associated with this page in case the application is suspended or the
        /// page is discarded from the navigation cache.  Values must conform to the serialization
        /// requirements of <see cref="SuspensionManager.SessionState"/>.
        /// </summary>
        /// <param name="pageState">An empty dictionary to be populated with serializable state.</param>
        protected override void SaveState(Dictionary<String, Object> pageState)
        {
        }
    }
}
