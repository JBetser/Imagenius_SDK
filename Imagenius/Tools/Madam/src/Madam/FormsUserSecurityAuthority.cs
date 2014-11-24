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
    using System.Diagnostics;
    using System.Net;
    using System.Web.Security;
    using System.Xml;
    using System.Xml.XPath;

    #endregion

    /// <summary>
    /// A user security authortiy implementation that uses the standard Forms 
    /// credential store and implementation supplied with ASP.NET.
    /// </summary>

    public sealed class FormsUserSecurityAuthority : UserSecurityAuthorityBase
    {
        private static IDictionary _credentialsByUser;
        private static readonly object _credentailsLock = new object();

        public FormsUserSecurityAuthority() {}
        
        public FormsUserSecurityAuthority(IDictionary settings) : 
            base(settings) {}

        protected override bool ValidateUser(string userName, string password)
        {
            return FormsAuthentication.Authenticate(userName, password);
        }

        protected override string GetPassword(string userName)
        {
            InitializeCredentialsByUser();
            NetworkCredential credential = (NetworkCredential)_credentialsByUser[userName];
            return credential != null ? credential.Password : null;
        }

        public override string GetUserPrivilege(string userName)
        {
            return null;
        }

        public override string GetUserShortName(string userName)
        {
            return null;
        }

        private static void InitializeCredentialsByUser()
        {
            lock (_credentailsLock)
            {
                if (_credentialsByUser == null)
                    _credentialsByUser = GetCredentialsByUser();
            }
        }

        public static NetworkCredential[] GetCredentials()
        {
            return GetCredentials(AppDomain.CurrentDomain.SetupInformation.ConfigurationFile);
        }

        public static NetworkCredential[] GetCredentials(string configPath)
        {
            if (configPath == null)
                throw new ArgumentNullException("configPath");

            XmlTextReader configReader = new XmlTextReader(configPath);

            try
            {
                return GetCredentials(configReader);
            }
            finally
            {
                configReader.Close();
            }
        }

        public static NetworkCredential[] GetCredentials(XmlReader configReader)
        {
            if (configReader == null)
                throw new ArgumentNullException("configReader");

            //
            // Unfortunately, System.Web.Security.FormsAuthentication provides
            // no programmatic access to the <credentials> section in the
            // configuration file so we have to scoop it out manually via the
            // XML API. Obviously there's a small risk here that changes to the
            // layout of the configuration file might cause this code to return
            // an empty result.
            //

            XPathDocument document = new XPathDocument(configReader);
            XPathNavigator navigator = document.CreateNavigator();

            //
            // Select the <user> nodes, but only if the configuration contains a
            // web-related authentication section that specifies (a) the mode
            // "Forms" and (b) that passwords are stored in clear text. Failure
            // to meet all of these conditions will cause this method to return
            // an empty array of credentials.
            //

            XPathNodeIterator iterator = navigator.Select(@"/
                configuration/
                    system.web/
                        authentication[translate(@mode,'FORMS','forms')='forms']/
                            forms/
                                credentials[translate(@passwordFormat,'CLEAR','clear')='clear']/
                                    user");

            //
            // Build a NetworkCredential array that contains an element for each
            // <user> node. NetworkCredential is used here rather than
            // introducing a new type that would serve pretty much the same
            // purpose. Also, we get the benefits of NetworkCredential's
            // implementation, which stores the password as an encrypted buffer
            // internally and also checks for caller permissions.
            //
    
            NetworkCredential[] credentials = new NetworkCredential[iterator.Count];
    
            while (iterator.MoveNext())
            {
                XPathNavigator user = iterator.Current;
                int index = iterator.CurrentPosition - 1;
                string userName = user.GetAttribute("name", string.Empty);
                string password = user.GetAttribute("password", string.Empty);
                credentials[index] = new NetworkCredential(userName, password);
            }

            if (credentials.Length == 0)
                Trace.WriteLine("WARNING! The Forms user security authority did not load any credentials from the configuration file. This can happen if the authentication mode is not configured for Forms, the <credentials> section is missing or empty, or that passwords are not specified to be stored in clear-text.");
    
            return credentials;
        }

        private static IDictionary GetCredentialsByUser()
        {
            return GetCredentialsByUser(GetCredentials());
        }

        private static IDictionary GetCredentialsByUser(NetworkCredential[] credentials)
        {
            //
            // Convert the array of credentials into a table of credentials
            // keyed by user name.
            //

            int capacity = credentials == null ? 0 : credentials.Length;
            Hashtable table = new Hashtable(capacity, CaseInsensitiveHashCodeProvider.DefaultInvariant, CaseInsensitiveComparer.DefaultInvariant);

            if (credentials != null)
            {
                foreach (NetworkCredential credential in credentials)
                    table.Add(credential.UserName, credential);
            }

            return table;
        }

#if TEST

        public static void SetCredentialsByUser(NetworkCredential[] credentials)
        {
            lock (_credentailsLock)
                _credentialsByUser = GetCredentialsByUser(credentials);
        }

#endif
    }
}
