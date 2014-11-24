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
    using System.Configuration;
    using System.Reflection;
    using System.Xml;
    using Debug = System.Diagnostics.Debug;

    #endregion

    /// <summary>
    /// Handler for the &lt;formsAuthenticationDisposition&gt; section of the
    /// configuration file.
    /// </summary>

    internal sealed class FormsAuthenticationDispositionSectionHandler : IConfigurationSectionHandler
    {
        public object Create(object parent, object configContext, XmlNode section)
        {
            //
            // Either inherit the incoming parent configuration (for example
            // from the machine configuration file) or start with a fresh new
            // one.
            //

            FormsAuthenticationDispositionConfiguration config;

            if (parent != null)
            {
                FormsAuthenticationDispositionConfiguration parentConfig = (FormsAuthenticationDispositionConfiguration) parent;
                config = (FormsAuthenticationDispositionConfiguration) parentConfig.Clone();
            }    
            else
            {
                config = new FormsAuthenticationDispositionConfiguration();
            }

            //
            // Treate the child <discriminators> section as a regular group.
            //

            XmlElement discriminatorsNode = section["discriminators"];

            if (discriminatorsNode != null)
            {
                Discriminator discriminator = new Discriminator();
                ((IConfigurationCallback) discriminator).Configure(discriminatorsNode);
                config.SetDiscriminator(discriminator);
            }

            return config;
        }
    }
}
