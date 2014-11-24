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

namespace IGMadam
{
    #region Imports

    using System;
    using System.Globalization;

    #endregion

    /// <summary>
    /// Provides helper methods for strings that should always be treated in the
    /// context of the invariant culture.
    /// </summary>

    internal sealed class InvariantString
    {
        private static readonly CultureInfo _invariant = CultureInfo.InvariantCulture;

        public static bool ContainsCaseless(string source, string sought)
        {
            return _invariant.CompareInfo.IndexOf(source, sought, CompareOptions.IgnoreCase) >= 0;
        }

        public static bool EndsWithCaseless(string source, string suffix)
        {
            return _invariant.CompareInfo.IsSuffix(source, suffix, CompareOptions.IgnoreCase);
        }

        public static bool EqualsCaseless(string s1, string s2)
        {
            return _invariant.CompareInfo.Compare(s1, s2, CompareOptions.IgnoreCase) == 0;
        }

        private InvariantString()
        {
            throw new NotSupportedException();
        }
    }
}