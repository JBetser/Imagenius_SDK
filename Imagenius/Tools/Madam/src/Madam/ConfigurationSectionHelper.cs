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
    using System.Configuration;
    using System.Diagnostics;
    using System.Xml;

    #endregion

    /// <summary>
    /// Helper class for handling values in configuration sections.
    /// </summary>

    internal sealed class ConfigurationSectionHelper
    {
        public static string GetValueAsString(XmlAttribute attribute)
        {
            return GetValueAsString(attribute, string.Empty);
        }

        public static string GetValueAsString(XmlAttribute attribute, string defaultValue)
        {
            if (attribute == null)
                return defaultValue;
            
            return Mask.EmptyString(attribute.Value, defaultValue);
        }

        public static bool GetValueAsBoolean(XmlAttribute attribute)
        {
            //
            // If the attribute is absent, then always assume the default value
            // of false. Not allowing the default value to be parameterized
            // maintains a consisent policy and makes it easier for the user to
            // remember that all boolean options default to false if not
            // specified.
            //

            if (attribute == null)
                return false;

            try
            {
                return XmlConvert.ToBoolean(attribute.Value);
            }
            catch (FormatException e)
            {
                throw new ConfigurationException(string.Format("Error in parsing the '{0}' attribute of the '{1}' element as a boolean value. Use either 1, 0, true or false (latter two being case-sensitive).", attribute.Name, attribute.OwnerElement.Name), e, attribute);
            }
        }
        
        private ConfigurationSectionHelper()
        {
            throw new NotSupportedException();
        }
    }
}