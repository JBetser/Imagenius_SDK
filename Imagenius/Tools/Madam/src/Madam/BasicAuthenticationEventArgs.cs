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
    using System.Security.Principal;
    using System.Web;

    #endregion

    /// <summary>
    /// Represents the method that will handle the 
    /// <see cref="BasicAuthenticationModule.Authenticate"/> event.
    /// </summary>

    public delegate void BasicAuthenticationEventHandler(object sender, BasicAuthenticationEventArgs e);

    /// <summary>
    /// Provides data for the 
    /// <see cref="BasicAuthenticationModule.Authenticate"/> event.
    /// </summary>

    public sealed class BasicAuthenticationEventArgs : EventArgs
    {
        private readonly HttpContext _context;
        private readonly IIdentity _identity;
        private readonly object _userToken;
        private IPrincipal _user;

        internal BasicAuthenticationEventArgs(HttpContext context, IIdentity identity, object userToken)
        {
            Debug.Assert(context != null);
            Debug.Assert(identity != null);
            Debug.Assert(userToken != null);

            _context = context;
            _identity = identity;
            _userToken = userToken;
        }

        public HttpContext Context
        {
            get { return _context; }
        }

        public IIdentity Identity
        {
            get { return _identity; }
        }

        public object UserToken
        {
            get { return _userToken; }
        }

        public IPrincipal User
        {
            get { return _user; }
            set { _user = value; }
        }
    }
}
