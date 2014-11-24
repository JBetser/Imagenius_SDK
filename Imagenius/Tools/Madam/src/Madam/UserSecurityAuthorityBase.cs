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
    using System.Security.Cryptography;
    using System.Security.Principal;
    using System.Text;
    using System.Web;

    #endregion

    /// <summary>
    /// A base implementation for a user security authority that also 
    /// implements <see cref="IUserPasswordProvider"/>.
    /// </summary>
    /// <remarks>
    /// At a minimum, a subclass must provide the clear text for a
    /// given user. If the downstream authority directly provides
    /// a user/password validation method, then it is recommended
    /// to call it in an override of <see cref="ValidateUser"/>.
    /// </remarks>

    public abstract class UserSecurityAuthorityBase : IUserSecurityAuthority, IUserPasswordProvider
    {
        private readonly string _realm;
        private readonly bool _exposeClearTextPasswords;

        public UserSecurityAuthorityBase() : this(null) {}

        public UserSecurityAuthorityBase(IDictionary settings)
        {
            _realm = SettingsHelper.PopString(settings, "realm", HttpRuntime.AppDomainAppVirtualPath);
            _exposeClearTextPasswords = SettingsHelper.PopBoolean(settings, "exposeClearTextPasswords");
        }

        /// <summary>
        /// Gets the clear-text password of the given user.
        /// </summary>

        protected abstract string GetPassword(string userName);
        public abstract string GetUserPrivilege(string userName);
        public abstract string GetUserShortName(string userName);

        protected virtual string GetPassword(string userName, ref string passwordFormat)
        {
            passwordFormat = null;
            return GetPassword(userName);
        }        
        
        /// <summary>
        /// The name of the realm protected by the this implementation.
        /// </summary>

        public virtual string RealmName
        {
            get { return _realm; }
        }

        /// <summary>
        /// Authenticates the username and password, given the format of the
        /// password and the authentication scheme.
        /// </summary>
        /// <remarks>
        /// IMPORTANT! Returns null if the authentication fails.
        /// </remarks>

        public virtual object Authenticate(string userName, object password, IDictionary options, string authenticationType)
        {
            string sPassword = null;
            string passwordFormat = null;
            string dbPassword = GetPassword(userName, ref passwordFormat);
            if (passwordFormat == PasswordFormat.ClearText.ToString() || passwordFormat == PasswordFormat.DigestAuthA1.ToString())
                sPassword = Mask.NullObject(password, string.Empty).ToString();
            else
                return null; 
            if ((dbPassword == null) || (sPassword == null) ||
                dbPassword != sPassword)
                return null;            
            return CreatePrincipal(userName, authenticationType, null);
        }

        /// <summary>
        /// Create an account for the specified username and password, given the format of the
        /// password and the authentication scheme. Account details are specified in the options.
        /// </summary>
        /// <remarks>
        /// The password may be either in a ClearText or a DigestAuthA1 format.
        /// return ACCOUNT_CREATION_SUCCEEDED if succeeded,
        /// otherwise an error code.
        /// </remarks>

        public virtual string CreateAccount(string userName, object password, PasswordFormat format, IDictionary options, bool isImplicitPassword)
        {
            return AccountCreationResult.CANNOT_ACCESS_DATABASE.ToString();
        }

        public virtual string ActivateAccount(string userName)
        {
            return AccountCreationResult.CANNOT_ACCESS_DATABASE.ToString();
        }

        public virtual string DisactivateAccount(string userName)
        {
            return AccountCreationResult.CANNOT_ACCESS_DATABASE.ToString();
        }


        /// <summary>
        /// Creates a principal for an authenticated user.
        /// </summary>
        /// <remarks>
        /// This method is not expected to check if the user is valid or not.
        /// </remarks>
        protected virtual IPrincipal CreatePrincipal(string userName, string authenticationType, string[] roles)
        {
            GenericIdentity identity = new GenericIdentity(userName, Mask.NullString(authenticationType));
            return new GenericPrincipal(identity, roles);
        }

        /// <summary>
        /// Validates the username and password using the clear-text password
        /// returned by <see cref="GetPassword"/>
        /// </summary>

        protected virtual bool ValidateUser(string userName, string password)
        {
            string actualPassword = GetPassword(userName);
            return actualPassword != null && actualPassword.Equals(password);
        }

        /// <summary>
        /// Retrieves the password of the user in the desired format
        /// or null if the user does not exist.
        /// </summary>

        public object GetPassword(string userName, PasswordFormat format, IDictionary options)
        {
            if (format == null)
                throw new ArgumentNullException("format");

            if (PasswordFormat.ClearText.Equals(format))
                return GetClearTextPassword(userName);

            if (PasswordFormat.DigestAuthA1.Equals(format))
                return GetDigestPassword(userName, options);

            throw new PasswordFormatNotSupportedException();
        }

        /// <summary>
        /// Determines whether a particular password format is supported 
        /// or not.
        /// </summary>

        bool IUserPasswordProvider.SupportsFormat(PasswordFormat format)
        {
            if (format == null)
                throw new ArgumentNullException("format");

            if (PasswordFormat.ClearText.Equals(format))
                return _exposeClearTextPasswords;

            return format.Equals(PasswordFormat.DigestAuthA1);
        }

        /// <summary>
        /// Determines if clear-text passwords should be revealed or exposed
        /// by this implementation through methods like 
        /// <see cref="GetPassword"/>.
        /// </summary>
 
        protected bool ExposeClearTextPasswords
        {
            get { return _exposeClearTextPasswords; }
        }

        /// <summary>
        /// Returns the clear-text password of the user or null if the user 
        /// is unknown. An exception is thrown if this implementation is
        /// configured to not expose clear-text passwords.
        /// </summary>

        protected virtual string GetClearTextPassword(string userName)
        {
            if (!_exposeClearTextPasswords)
                throw new PasswordFormatNotSupportedException(string.Format("The clear-text password format is not supported."));

            return GetPassword(userName);
        }

        /// <summary>
        /// Returns the password of the user in the digest-auth-a1 format
        /// or null if the user is unknown.
        /// </summary>

        protected virtual string GetDigestPassword(string userName, IDictionary options)
        {
            string password = GetPassword(userName);

            if (password == null)
                return null;

            return CryptPassword(password, userName);
        }

        /// <summary>
        /// Crypt the password of the user in the digest-auth-a1 format
        /// </summary>

        protected string CryptPassword(string password, string userName)
        {
            return UserSecurityAuthority.GetPasswordDigest(userName, RealmName, password);
        }
    }
}