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
using System.Linq;

namespace IGMadam
{
    #region Imports

    using System;
    using System.Collections;
    using System.Diagnostics;
    using System.Security.Cryptography;
    using System.Text;
    using System.Web;
    using System.Collections.Generic;

    #endregion

    /// <summary>
    /// Provides access to the configured user security authority as well as
    /// a few helper methods for user security authorities. This class cannot
    /// be inherited.
    /// </summary>

    public sealed class UserSecurityAuthority
    {
        private static readonly IUserSecurityAuthority _nullAuthority = new UserSecurityNullAuthority();
        [ ThreadStatic ] private static IUserSecurityAuthority _authority;

        public static IUserSecurityAuthority FromConfig()
        {
            // 
            // To register an IUserSecurityAuthority implementation in the
            // configuration file, ensure that the madam/userSecurityAuthority
            // section is registered as follows:
            //
            // <configSections>
            //     <sectionGroup name="madam">
            //        <section name="userSecurityAuthority" type="System.Configuration.SingleTagSectionHandler, System, Version=1.0.5000.0, Culture=neutral, PublicKeyToken=b77a5c561934e089" />
            //     </sectionGroup>
            // </configSections>
            //
            // Then add the actual section with the provider attribute of the
            // <userSecurityAuthority> section reflecting the assembly-qualified
            // type name of the implementation:
            //
            // <madam>
            //    <userSecurityAuthority 
            //      provider="ASSEMBLY_QUALIFIED_TYPE_NAME" />
            // </madam>
            //

            if (_authority == null)
            {
                IUserSecurityAuthority authority = (IUserSecurityAuthority)ServiceProviderFactory.CreateFromConfigSection("IGMadam/userSecurityAuthority", "provider");

                if (authority == null)
                {
                    Trace.WriteLine("WARNING! There appears to be no user security authority configured. MADAM will use a null authority that defines no users. This will effectively cause all authentications to fail.");
                    authority = _nullAuthority;
                }
                
                _authority = authority;
            }

            return _authority;
        }
        
        public static string GetPasswordDigest(string userName, string realm, string password)
        {
            return EncodePassword(userName + realm + password);
        }
        
        private static string ToLowerCaseHexString(byte[] hash)
        {
            return hash
                .Select(b => String.Format("{0:x2}",
                                           b))
                .Aggregate((a, b) => a + b);
        }

        public static string EncodePassword(string originalPassword)
        {
            //Declarations
            Byte[] originalBytes;
            Byte[] encodedBytes;
            SHA256 sha256;

            //Instantiate SHA256CryptoServiceProvider, get bytes for original password and compute hash (encoded password)
            sha256 = new SHA256CryptoServiceProvider();
            originalBytes = Encoding.Default.GetBytes(originalPassword);
            encodedBytes = sha256.ComputeHash(originalBytes);

            //Convert encoded bytes back to a 'readable' string
            return ToLowerCaseHexString(encodedBytes);
        }
        
        private UserSecurityAuthority()
        {
            throw new NotSupportedException();
        }

        private class UserSecurityNullAuthority : IUserSecurityAuthority
        {
            public string RealmName
            {
                get { return HttpRuntime.AppDomainAppVirtualPath; }
            }

            public object Authenticate(string userName, object password, IDictionary options, string authenticationType)
            {
                return null;
            }

            public string CreateAccount(string userName, object password, PasswordFormat format, IDictionary options, bool isImplicitPassword)
            {
                return AccountCreationResult.FAILED_FOR_UNKNOWN_REASON.ToString();
            }

            public string ActivateAccount(string userName)
            {
                return AccountCreationResult.FAILED_FOR_UNKNOWN_REASON.ToString();
            }

            public string DisactivateAccount(string userName)
            {
                return AccountCreationResult.FAILED_FOR_UNKNOWN_REASON.ToString();
            }

            public string GetServerName(string userName)
            {
                return null;
            }

            public string GetUserPrivilege(string userName)
            {
                return null;
            }

            public string GetUserShortName(string userName)
            {
                return null;
            }
        }
    }
}
