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
    using System.Collections;
    using System.Xml;

    #endregion

    /// <summary>
    /// Provides convenience methods for popping settings off a dictionary.
    /// If the dictionary is null or a specified key is not found then
    /// a default value is always returned instead. The key, if found, is
    /// always removed from the dictionary.
    /// </summary>

    public sealed class SettingsHelper
	{
        public static string PopString(IDictionary settings, string key)
        {
            return PopString(settings, key, string.Empty);
        } 

        public static string PopString(IDictionary settings, string key, string defaultValue)
        {
            if (settings == null)
                return defaultValue;

            string value = (string) settings[key];
            settings.Remove(key);

            return Mask.EmptyString(value, defaultValue);
        }

        public static bool PopBoolean(IDictionary settings, string key)
        {
            return XmlConvert.ToBoolean(PopString(settings, key, "0"));
        }

        private SettingsHelper()
		{
            throw new NotSupportedException();
		}
	}
}
