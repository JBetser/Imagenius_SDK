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
	using System.Diagnostics;
	using IDictionary = System.Collections.IDictionary;
    using ConfigurationSettings = System.Configuration.ConfigurationSettings;

	#endregion

    /// <summary>
    /// A simple factory for creating instances of types specified in a 
    /// section of the configuration file.
    /// </summary>
	
    internal sealed class ServiceProviderFactory
	{
        /// <summary>
        ///  Creates an object from a configuration section, using its
        ///  "type" attribute as the type specification of the object's 
        ///  type. The configuration section must yield a dictionary 
        ///  object.
        /// </summary>

        public static object CreateFromConfigSection(string sectionName)
        {
            return CreateFromConfigSection(sectionName, null);
        }

        /// <summary>
        ///  Creates an object from a configuration section, using a
        ///  given attribute as the type specification of the object's type.
        ///  The configuration section must yield a dictionary object.
        /// </summary>
        
        public static object CreateFromConfigSection(string sectionName, string typeAttributeName)
        {
            Debug.Assert(Mask.NullString(sectionName).Length > 0);

            IDictionary config = (IDictionary) ConfigurationSettings.GetConfig(sectionName);
            return CreateFromConfig(config, typeAttributeName);
        }

        /// <summary>
        /// Creates an object given its text-based type specification 
        /// (see <see cref="System.Type.GetType"/> for notes on the
        /// specification) and optionally sends it settings for
        /// initialization.
        /// </summary>

        public static object Create(string typeSpec, IDictionary settings)
        {
            if (typeSpec.Length == 0)
                return null;

            Type type = Type.GetType(typeSpec, true);

            return (settings == null) ? 
                Activator.CreateInstance(type) :
                Activator.CreateInstance(type, new object[] { settings });
        }

        /// <summary>
        /// Creates an object and optionally initializes it with a given
        /// set of settings. A key can be supplied that specifies which 
        /// entry in the configuration dictionary identifies the type of 
        /// the object to be created. A null or empty key means the 
        /// default key "type" will be used.
        /// </summary>

        private static object CreateFromConfig(IDictionary config, string typeKey)
        {
            if (config == null)
                return null;
    
            //
            // We modify the settings by removing items as we consume 
            // them so make a copy here.
            //
    
            config = (IDictionary) ((ICloneable) config).Clone();
    
            //
            // Get the type specification of the object to create.
            //
    
            typeKey = Mask.EmptyString(typeKey, "type");
            string typeSpec = Mask.NullString((string) config[typeKey]);
            config.Remove("type");

            //
            // Create the object!
            //

            return Create(typeSpec, config);
        }

        private ServiceProviderFactory()
        {
            throw new NotSupportedException();
        }
	}
}
