#region License, Terms and Conditions
//
// The zlib/libpng License 
// Copyright (c) 2006, Atif Aziz, Skybow AG. All rights reserved.
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not claim
//    that you wrote the original software. If you use this software in a 
//    product, an acknowledgment in the product documentation would be 
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
#endregion

#region Imports

using System.Reflection;

using CLSCompliantAttribute = System.CLSCompliantAttribute;
using ComVisible = System.Runtime.InteropServices.ComVisibleAttribute;

#endregion

//
// General description
//

[assembly: AssemblyTitle("MADAM")]
[assembly: AssemblyDescription("MADAM for ASP.NET")]
[assembly: AssemblyCompany("")]
[assembly: AssemblyProduct("MADAM")]
[assembly: AssemblyCopyright("Copyright (c) 2006, Atif Aziz, Skybow AG. All rights reserved.")]
[assembly: AssemblyCulture("")]

//
// Version information
//

[assembly: AssemblyVersion("1.0.7510.0")]
[assembly: AssemblyFileVersion("1.0.7510.0")]

//
// Configuration (test, debug, release)
//

#if TEST
    #if !DEBUG
    #warning Test builds should be compiled using the DEBUG configuration.
    #endif
    [assembly: AssemblyConfiguration("Test")]
#elif DEBUG
[assembly: AssemblyConfiguration("Debug")]
#else
    [assembly: AssemblyConfiguration("Release")]
#endif

//
// COM visibility and CLS compliance
//

[assembly: CLSCompliant(true)] 
[assembly: ComVisible(false)]
