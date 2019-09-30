//------------------------------------------------------------------------------
// <copyright file="ItemsControlsSample.xaml.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.Samples.Kinect.ControlsBasics
{
    using System.Collections.Generic;
    using System.Windows.Controls;
    using System.Windows.Documents;
    
    /// <summary>
    /// Interaction logic for ItemsControlsSample
    /// </summary>
    public partial class ItemsControlsSample : UserControl
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="ItemsControlsSample" /> class.
        /// </summary>
        public ItemsControlsSample()
        {
            this.InitializeComponent();
            this.Loaded += this.ItemsControlsSample_Loaded;
        }

        /// <summary>
        /// UserControl loaded handler
        /// </summary>
        /// <param name="sender">UserControl which raises event.</param>
        /// <param name="e">EventArgs for this routed event.</param>
        private void ItemsControlsSample_Loaded(object sender, System.Windows.RoutedEventArgs e)
        {
            List<Item> items = new List<Item>();
            for (int i = 1; i <= 25; i++)
            {
                items.Add(new Item() { Name = "Item " + i });
            }

            itemListView.ItemsSource = items;
            itemListBox.ItemsSource = items;
        }

        /// <summary>
        /// Data item
        /// </summary>
        private class Item
        {
            /// <summary>
            /// Gets or sets the name of item.
            /// </summary>
            public string Name { get; set; }
        }
    }
}
