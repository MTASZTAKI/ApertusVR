//------------------------------------------------------------------------------
// <copyright file="GlobalSuppressions.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// This file is used to define SuppressMessage attributes that apply to all
// C# samples.
//
// Don't add more suppressions to this file and don't add additional
// suppression files to individual projects. Released samples should be
// FxCop clean other than this very limited set of exceptions.

[assembly: System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA2210:AssembliesShouldHaveValidStrongNames", Justification = "Samples will not be signed")]
[assembly: System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1020:AvoidNamespacesWithFewTypes", Scope = "namespace", Target = "XamlGeneratedNamespace", Justification = "Samples may have only a few types in a namespace")]
[assembly: System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1062:Validate arguments of public methods", MessageId = "0", Scope = "member", Target = "XamlGeneratedNamespace.GeneratedInternalTypeHelper.#AddEventHandler(System.Reflection.EventInfo,System.Object,System.Delegate)", Justification = "Generated code")]
[assembly: System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1062:Validate arguments of public methods", MessageId = "1", Scope = "member", Target = "XamlGeneratedNamespace.GeneratedInternalTypeHelper.#CreateDelegate(System.Type,System.Object,System.String)", Justification = "Generated code")]
[assembly: System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1062:Validate arguments of public methods", MessageId = "0", Scope = "member", Target = "XamlGeneratedNamespace.GeneratedInternalTypeHelper.#GetPropertyValue(System.Reflection.PropertyInfo,System.Object,System.Globalization.CultureInfo)", Justification = "Generated code")]
[assembly: System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1062:Validate arguments of public methods", MessageId = "0", Scope = "member", Target = "XamlGeneratedNamespace.GeneratedInternalTypeHelper.#SetPropertyValue(System.Reflection.PropertyInfo,System.Object,System.Object,System.Globalization.CultureInfo)", Justification = "Generated code")]
