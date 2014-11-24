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
    using System.Data;
    using System.Globalization;
    using System.IO;
    using System.Text.RegularExpressions;
    using System.Web;

    #endregion

    /// <summary>
    /// A user security authortiy implementation that uses ADO.NET to 
    /// interface with a database used as the credentials store.
    /// </summary>

    public sealed class DatabaseUserSecurityAuthority : UserSecurityAuthorityBase
    {
        public static readonly string IGMADAM_USERNAME = "UserName";
        public static readonly string IGMADAM_USERSHORTNAME = "UserShortName";
        public static readonly string IGMADAM_USERIP = "UserIP";
        public static readonly string IGMADAM_USERPRIVILEGE = "UserRights";
        public static readonly string IGMADAM_USERPRIVILEGE_DEMO = "Demo";
        public static readonly string IGMADAM_USERPRIVILEGE_GUEST = "Guest";
        public static readonly string IGMADAM_USERPRIVILEGE_LIMITED = "Limited";
        public static readonly string IGMADAM_USERPRIVILEGE_GOLD = "Gold";
        public static readonly string IGMADAM_USERPRIVILEGE_ADMIN = "Admin";
        public static readonly string IGMADAM_USERPRIVILEGE_ACTIVATOR = "Activator";
        public static readonly string IGMADAM_ACTIVATIONSTATUS = "ActivationStatus";
        public static readonly string IGMADAM_ACTIVATIONSTATUS_ACTIVATED = "Activated";
        public static readonly string IGMADAM_ACTIVATIONSTATUS_DISACTIVATED = "Disactivated";
        public static readonly string IGMADAM_ACTIVATIONSTATUS_PENDING = "Pending";
        public static readonly string IGMADAM_PASSWORD = "Password";
        public static readonly string IGMADAM_PASSWORDFORMAT = "PasswordFormat";
        public static readonly string IGMADAM_PUBLICKEY = "F6pwd";

        private readonly string _connectionString;
        private readonly Type _connectionType;
        private readonly string _commandText;
        private readonly CommandType _commandType;
        private readonly string _userParameterName;

        public const int MIN_USERNAME_LENGTH = 3;
        public const int MAX_USERNAME_LENGTH = 64;
        public const int MIN_PASSWORD_LENGTH = 3;
        public const int MAX_PASSWORD_LENGTH = 256;

        public DatabaseUserSecurityAuthority() : this(null) {}

        public DatabaseUserSecurityAuthority(IDictionary settings) :
            base(settings)
        {
            //
            // Get the type specification of the connection object to use and
            // crack it into a Type instance.

            string connectionTypeSpec = SettingsHelper.PopString(settings, "connectionType");

            if (connectionTypeSpec.Length == 0)
                throw new ArgumentException("Settings does not specify a connection type.", "settings");

            //
            // The type specification can a simple or an assembly-qualified 
            // type name. In the case of a simple type name, where no comma
            // that delimits the type name from an assembly specification is
            // found, we make an additional search in the System.Data 
            // assembly. This allows simple type specification for common
            // types like System.Data.SqlClient.SqlConnection without needing
            // to specify the strong assembly name.
            //
            // NOTE: For sake of simplicity, there is no check to support 
            // edge and exotic cases where the comma may be escaped.
            //

            if (connectionTypeSpec.IndexOf(',') < 0)
                _connectionType =  typeof(IDbConnection).Assembly.GetType(connectionTypeSpec, /* throwOnError */ false);

            if (_connectionType == null)
                Type.GetType(connectionTypeSpec, /* throwOnError */ true);

            //
            // Is the resolved type indeed an ADO.NET connection type?
            //

            if (!typeof(IDbConnection).IsAssignableFrom(_connectionType))
                throw new ArgumentException(string.Format("Settings specifies a connection type {0} that is not compatible with {1}.", _connectionType.FullName, typeof(IDbConnection).FullName), "settings");

            //
            // Get other mandatory settings like the connection string 
            // and command text.
            //

            _connectionString = ExpandConnectionStringMacros(SettingsHelper.PopString(settings, "connectionString"));

            if (_connectionString.Length == 0)
                throw new ArgumentException("Settings does not specify a connection string.", "settings");

            _commandText = SettingsHelper.PopString(settings, "query");
        
            if (_commandText.Length == 0)
                throw new ArgumentException("Settings does not specify a query.", "settings");

            //
            // Get the optional settings. Note that if the user parameter 
            // name is not supplied then we assume the command object will
            // be happy with positional parameters.
            //

            _commandType = SettingsHelper.PopBoolean(settings, "queryIsProc") ? CommandType.StoredProcedure : CommandType.Text;
            _userParameterName = SettingsHelper.PopString(settings, "userParameter");
        }

        public string ConnectionString
        {
            get { return _connectionString; }
        }   
        
        /// <summary>
        /// Returns the specified user's password value by reading it from
        /// the database, otherwise null if the user does not exist.
        /// </summary>
        /// <remarks>
        /// This method assumes that the password is in a clear format.
        /// </remarks>

        protected override string GetPassword(string userName)
        {
            string passwordFormat = null;
            return GetPassword(userName, false, ref passwordFormat);
        }

        protected override string GetPassword(string userName, ref string passwordFormat)
        {
            return GetPassword(userName, true, ref passwordFormat);
        }

        private string GetPassword(string userName, bool bGetPasswordFormat, ref string passwordFormat)
        {
            object password, format;

            using (IDbConnection connection = (IDbConnection) Activator.CreateInstance(_connectionType))
            {
                connection.ConnectionString = ConnectionString;
                IDbCommand command = connection.CreateCommand();
                command.CommandText = _commandText;
                command.CommandType = _commandType;
        
                IDataParameter parameter = command.CreateParameter();
                parameter.ParameterName = _userParameterName;
                parameter.Value = userName;
                command.Parameters.Add(parameter);

                parameter = command.CreateParameter();
                parameter.ParameterName = IGMADAM_ACTIVATIONSTATUS;
                parameter.Value = IGMADAM_ACTIVATIONSTATUS_ACTIVATED;
                command.Parameters.Add(parameter);

                command.Connection.Open();
                password = command.ExecuteScalar();

                if (bGetPasswordFormat)
                {
                    command.CommandText = "SELECT " + IGMADAM_PASSWORDFORMAT + " FROM Users WHERE " + IGMADAM_USERNAME + "=@" + IGMADAM_USERNAME + " AND " + IGMADAM_ACTIVATIONSTATUS + "=@" + IGMADAM_ACTIVATIONSTATUS;
                    format = command.ExecuteScalar();
                    if (format == null)
                        return null;
                    passwordFormat = format.ToString();
                }
            }

            if (password == null || Convert.IsDBNull(password))
                return null;

            return password.ToString();
        }

        public override string GetUserShortName(string userName)
        {
            return SelectFromUser(IGMADAM_USERSHORTNAME, userName);
        }

        public override string GetUserPrivilege(string userName)
        {
            return SelectFromUser(IGMADAM_USERPRIVILEGE, userName);
        }

        private string SelectFromUser(string userPropType, string userName)
        {
            object objUserProp = null;

            using (IDbConnection connection = (IDbConnection)Activator.CreateInstance(_connectionType))
            {
                connection.ConnectionString = ConnectionString;
                IDbCommand command = connection.CreateCommand();
                command.CommandText = _commandText;
                command.CommandType = _commandType;

                IDataParameter parameter = command.CreateParameter();
                parameter.ParameterName = _userParameterName;
                parameter.Value = userName;
                command.Parameters.Add(parameter);

                command.CommandText = "SELECT " + userPropType + " FROM Users WHERE " + IGMADAM_USERNAME + "=@" + IGMADAM_USERNAME;
                command.Connection.Open();
                objUserProp = command.ExecuteScalar();
            }

            if (objUserProp == null)
                return null;

            return objUserProp.ToString();
        }

        /// <summary>
        /// Add a new user account into the database return ACCOUNT_CREATION_SUCCEEDED if succeeded,
        /// otherwise an error code.
        /// </summary>

        public override string CreateAccount(string userName, object pwd, PasswordFormat format, IDictionary options, bool isImplicitPassword)
        {
            if (userName == null)
                throw new ArgumentNullException("userName");

            string password = pwd as string;
            if (password == null)
                throw new ArgumentNullException("password");

            if (format == null)
                throw new ArgumentNullException("format");

            if (options == null)
                throw new ArgumentNullException("options");

            if ((userName.Length < MIN_USERNAME_LENGTH) || (password.Length < MIN_PASSWORD_LENGTH) ||
                (userName.Length > MAX_USERNAME_LENGTH) || (password.Length > MAX_PASSWORD_LENGTH))
                return AccountCreationResult.INVALID_PARAMETERS.ToString();

            // first test if the account already exists
            if (GetPassword(userName, format, options) != null)
                return AccountCreationResult.ACCOUNT_ALREADY_EXISTS.ToString();

            bool bClear = true;
            if (PasswordFormat.DigestAuthA1.Equals(format))
            {
                bClear = false;
            }
            else
            {
                if (!PasswordFormat.ClearText.Equals(format))
                    return AccountCreationResult.INVALID_PARAMETERS.ToString();
            }

            string shortUserName = SettingsHelper.PopString(options, IGMADAM_USERSHORTNAME);
            if ((shortUserName == null) || (shortUserName.Length == 0))
            {
                int nIdxOfAr = userName.IndexOf('@');
                if (nIdxOfAr < 0)
                    shortUserName = userName;
                else
                    shortUserName = userName.Substring(0, nIdxOfAr);
            }
            else if (shortUserName.Contains("@"))
                shortUserName = shortUserName.Split('@')[0];
            string userRights = SettingsHelper.PopString(options, IGMADAM_USERPRIVILEGE);
            if ((userRights == null) || (userRights.Length == 0))
                return AccountCreationResult.INVALID_PARAMETERS.ToString();

            if (userRights == DatabaseUserSecurityAuthority.IGMADAM_USERPRIVILEGE_GUEST){
                if (isImplicitPassword)
                {
                    string hash = UserSecurityAuthority.GetPasswordDigest(shortUserName, RealmName, DatabaseUserSecurityAuthority.IGMADAM_PUBLICKEY + userName.Split('@')[0]);
                    if (password != hash)
                        return AccountCreationResult.INVALID_PARAMETERS.ToString();
                }
                else
                    password = UserSecurityAuthority.GetPasswordDigest(shortUserName, RealmName, DatabaseUserSecurityAuthority.IGMADAM_PUBLICKEY + password);
            }

            try
            {
                using (IDbConnection connection = (IDbConnection)Activator.CreateInstance(_connectionType))
                {
                    connection.ConnectionString = ConnectionString;
                    IDbCommand command = connection.CreateCommand();
                    command.CommandText = "INSERT INTO Users(" + IGMADAM_USERNAME + ", [" + IGMADAM_PASSWORD + "], [" + IGMADAM_USERSHORTNAME + "], [" + IGMADAM_PASSWORDFORMAT + "], [" + IGMADAM_USERPRIVILEGE + "], [" + IGMADAM_ACTIVATIONSTATUS + "]) VALUES(";
                    command.CommandText += "'" + userName + "', ";
                    command.CommandText += "'" + password + "', ";
                    command.CommandText += "'" + shortUserName + "', ";
                    command.CommandText += "'" + (bClear ? PasswordFormat.ClearText.ToString() :
                                                            PasswordFormat.DigestAuthA1.ToString()) + "', ";
                    command.CommandText += "'" + userRights + "',";
                    command.CommandText += "'" + IGMADAM_ACTIVATIONSTATUS_PENDING + "');";
                    command.CommandType = _commandType;
                    command.Connection.Open();
                    command.ExecuteScalar();
                }
            }
            catch (Exception exc)
            {
                return exc.ToString();
            }
            return AccountCreationResult.ACCOUNT_CREATION_SUCCEEDED.ToString();
        }

        public override string ActivateAccount(string userName)
        {
            return internalActivateAccount(userName, true);
        }

        public override string DisactivateAccount(string userName)
        {
            return internalActivateAccount(userName, false);
        }

        private string internalActivateAccount(string userName, bool activate)
        {
            try
            {
                using (IDbConnection connection = (IDbConnection)Activator.CreateInstance(_connectionType))
                {
                    connection.ConnectionString = ConnectionString;
                    IDbCommand command = connection.CreateCommand();
                    command.CommandText = "SELECT " + IGMADAM_USERSHORTNAME + " FROM Users ";
                    command.CommandText += "WHERE " + IGMADAM_USERNAME + " = '" + userName + "'";
                    command.CommandType = _commandType;
                    command.Connection.Open();
                    if (command.ExecuteScalar() == null)
                        return AccountCreationResult.ACCOUNT_DOES_NOT_EXIST.ToString();
                }

                using (IDbConnection connection = (IDbConnection)Activator.CreateInstance(_connectionType))
                {
                    connection.ConnectionString = ConnectionString;
                    IDbCommand command = connection.CreateCommand();
                    command.CommandText = "UPDATE Users SET " + IGMADAM_ACTIVATIONSTATUS + " = ";
                    command.CommandText += "'" + (activate ? IGMADAM_ACTIVATIONSTATUS_ACTIVATED : IGMADAM_ACTIVATIONSTATUS_DISACTIVATED) + "' ";
                    command.CommandText += "WHERE " + IGMADAM_USERNAME + " = '" + userName + "'";
                    command.CommandType = _commandType;
                    command.Connection.Open();
                    command.ExecuteScalar();
                }
            }
            catch (Exception exc)
            {
                return exc.ToString();
            }
            return AccountCreationResult.ACCOUNT_CREATION_SUCCEEDED.ToString();
        }

        /// <summary>
        /// Resolves the |DataDirectory| macro, if it exists.
        /// </summary>
        /// <remarks>
        /// This is mainly provided for compatibility between ASP.NET 1.x
        /// and 2.0, where the web site may share the same structure.
        /// </remarks>
        
        public static string ExpandConnectionStringMacros(string connectionString)
        {
            if (Mask.NullString(connectionString).Length == 0)
                return string.Empty;

            const string dataDirectory = "|DataDirectory|";

            int index = connectionString.IndexOf(dataDirectory);

            if (index < 0)
                index = CultureInfo.InvariantCulture.CompareInfo.IndexOf(connectionString, dataDirectory, CompareOptions.IgnoreCase);

            if (index < 0)
                return connectionString;

            //
            // In ASP.NET 2.0, the data directory (usually App_Data) is 
            // stored in the AppDomain data under the key "DataDirectory".
            // This fact is presently undocumented in MSDN, but discussed 
            // in the following blog entry:
            //
            // http://blogs.msdn.com/smartclientdata/archive/2005/08/26/456886.aspx
            //
            // Rather than relying on this entirely, this method will
            // default to "~/App_Data" in the absence of "DataDirectory"
            // in the AppDomain data (which is how it will behave for
            // ASP.NET 1.x applications).
            //

            string appDataPath = Mask.EmptyString((string) AppDomain.CurrentDomain.GetData("DataDirectory"), Path.Combine(HttpRuntime.AppDomainAppPath, "App_Data\\"));
            return connectionString.Substring(0, index) + appDataPath + connectionString.Substring(index + dataDirectory.Length);
        }
    }
}
