//-----------------------------------------------------------------------
// <copyright file="AssemblyInfo.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>
// <summary>
// Assembly attributes.
// </summary>
//-----------------------------------------------------------------------
using System;
using System.Reflection;
using System.Resources;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

// General Information about an assembly is controlled through the following 
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
[assembly: AssemblyTitle("Microsoft.Kinect.Fusion")]
[assembly: AssemblyDescription("")]
[assembly: AssemblyConfiguration("")]

// The following GUID is for the ID of the typelib if this project is exposed to COM
[assembly: Guid("84ddb6f2-adc1-42f2-b180-b1894af8d183")]

[assembly: NeutralResourcesLanguageAttribute("en-US")]

[assembly: CLSCompliant(true)] // Only works if interop references do not embed Interop types, otherwise we get non-compliant COM components from native code interop assembly

[assembly: ComVisible(false)]