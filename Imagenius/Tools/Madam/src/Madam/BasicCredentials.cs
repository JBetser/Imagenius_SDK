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
    /// <summary>
    /// Credentials for Basic authentication.
    /// </summary>
    
    public class BasicCredentials
    {
        private readonly string _userName;
        private readonly string _password;

        public BasicCredentials(string userName, string password)
        {
            _userName = userName;
            _password = password;
        }

        public virtual string UserName
        {
            get { return Mask.NullString(_userName); }
        }

        public virtual string Password
        {
            get { return Mask.NullString(_password); }
        }
    }
}