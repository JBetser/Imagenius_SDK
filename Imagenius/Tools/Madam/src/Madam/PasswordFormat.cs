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

    #endregion

    /// <summary>
    /// Represents a named password format that may be supported by various <see
    /// cref="IUserPasswordProvider"/> implementations.
    /// </summary>

    [ Serializable ]
    public sealed class PasswordFormat
    {
        public readonly static PasswordFormat ClearText = new PasswordFormat("clear-text");
        public readonly static PasswordFormat DigestAuthA1 = new PasswordFormat("digest-auth-a1");

        private readonly string _name;

        public PasswordFormat(string name)
        {
            if (name == null)
                throw new ArgumentNullException("name");

            if (name.Length == 0)
                throw new ArgumentOutOfRangeException("name");

            _name = name;
        }

        public static PasswordFormat FromName(string name)
        {
            if (ClearText.EqualsName(name))
                return PasswordFormat.ClearText;
            else if (DigestAuthA1.EqualsName(name))
                return PasswordFormat.DigestAuthA1;
            else
                return new PasswordFormat(name);
        }

        public string Name
        {
            get { return _name; }
        }

        public override int GetHashCode()
        {
            return _name.GetHashCode();
        }

        public override bool Equals(object obj)
        {
            if (obj == null)
                return false;

            if (object.ReferenceEquals(this, obj))
                return true;

            PasswordFormat other = obj as PasswordFormat;

            if (other == null)
                return false;

            return EqualsName(other._name);
        }

        public bool EqualsName(string name)
        {
            if (name == null)
                return false;

            return InvariantString.EqualsCaseless(_name, name);
        }
            
        public override string ToString()
        {
            return _name;
        }
    }
}
