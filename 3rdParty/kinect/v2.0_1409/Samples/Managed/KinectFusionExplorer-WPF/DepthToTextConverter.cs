//------------------------------------------------------------------------------
// <copyright file="DepthToTextConverter.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.Samples.Kinect.KinectFusionExplorer
{
    using System;
    using System.Globalization;
    using System.Windows.Data;

    /// <summary>
    /// Convert depth to UI text
    /// </summary>
    public class DepthToTextConverter : IValueConverter
    {
        /// <summary>
        /// Converts from depth value to a text value.
        /// </summary>
        /// <param name="value">The depth value.</param>
        /// <param name="targetType">The target type.</param>
        /// <param name="parameter">Any parameter supplied to the converter.</param>
        /// <param name="culture">The CultureInfo to use.</param>
        /// <returns>A string representing the depth value.</returns>
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            return ((double)value).ToString("0.00", CultureInfo.CurrentCulture) + "m";
        }

        /// <summary>
        /// This method is not implemented.
        /// </summary>
        /// <param name="value">The depth value.</param>
        /// <param name="targetType">The target type.</param>
        /// <param name="parameter">Any parameter supplied to the converter.</param>
        /// <param name="culture">The CultureInfo to use.</param>
        /// <returns>Nothing because it throws a NotImplementedException.</returns>
        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
