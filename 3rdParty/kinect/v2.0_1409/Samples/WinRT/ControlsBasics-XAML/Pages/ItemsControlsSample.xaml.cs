//------------------------------------------------------------------------------
// <copyright file="ItemsControlsSample.xaml.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Microsoft.Samples.Kinect.ControlsBasics.Data;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace Microsoft.Samples.Kinect.ControlsBasics
{
    /// <summary>
    /// A basic page that provides characteristics common to most applications.
    /// </summary>
    public sealed partial class ItemsControlsSample : Microsoft.Samples.Kinect.ControlsBasics.Common.LayoutAwarePage
    {
        public ItemsControlsSample()
        {
            this.InitializeComponent();
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
            var smallListViewItems = SampleDataSource.GetGroups("AllGroups").Where(g => g.UniqueId.Equals("Group-1")).ToList();
            
            // Cloning the data set over and over again to increase its size
            List<SampleDataGroup> largeListViewItems = SampleDataSource.GetGroups("AllGroups").Where(g => !g.UniqueId.Equals("Group-1")).ToList<SampleDataGroup>();
            largeListViewItems.AddRange(largeListViewItems);
            largeListViewItems.AddRange(largeListViewItems);
            largeListViewItems.AddRange(largeListViewItems);
                        
            this.DefaultViewModel["SmallList"] = smallListViewItems;
            this.DefaultViewModel["LargeList"] = largeListViewItems;
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
