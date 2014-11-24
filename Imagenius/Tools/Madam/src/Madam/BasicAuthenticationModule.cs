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
    using System.Security.Principal;
    using System.Text;
    using System.Web;

    #endregion

    /// <summary>
    /// Enables ASP.NET applications to use Basic authentication as specified in
    /// RFC 2617 (HTTP Authentication: Basic and Digest Access Authentication).
    /// </summary>

    public class BasicAuthenticationModule : IHttpModule
    {
        public const string SchemeName = "Basic";

        private const string _basicSpace = SchemeName + " ";

        private Encoding _credentialsEncoding;

        public event BasicAuthenticationEventHandler Authenticate;

        public virtual void Init(HttpApplication context)
        {
            if (context == null)
                throw new ArgumentNullException("context");

            //
            // Subscribe to events.
            //

            context.AuthenticateRequest += new EventHandler(OnAuthenticateRequest);
            context.EndRequest += new EventHandler(OnEndRequest);
        }

        public virtual void Dispose()
        {
        }

        private void OnAuthenticateRequest(object sender, EventArgs e)
        {
            HttpApplication application = (HttpApplication) sender;
            AuthenticateRequest(application.Context);
        }

        private void OnEndRequest(object sender, EventArgs e)
        {
            HttpApplication application = (HttpApplication) sender;

            if (ShouldChallenge(application.Context))
                IssueChallenge(application.Context);
        }

        /// <summary>
        /// Gets the realm to issue in the challenge.
        /// </summary>

        protected virtual string GetRealm(HttpContext context)
        {
            return UserSecurityAuthority.FromConfig().RealmName;
        }

        /// <summary>
        /// Determines whether to issue the challenge or not.
        /// </summary>

        protected virtual bool ShouldChallenge(HttpContext context)
        {
            if (context == null)
                throw new ArgumentNullException("context");

            return context.Response.StatusCode == 401;
        }

        /// <summary>
        /// Authenticates the authorization request.
        /// </summary>

        protected virtual void AuthenticateRequest(HttpContext context)
        {
            if (context == null)
                throw new ArgumentNullException("context");

            if (context.Request.IsAuthenticated)
                return;

            //
            // Parse and obtain the credentials.
            //

            BasicCredentials credentials = GetCredentials(context);
            
            if (credentials == null)
                return;

            //
            // Allow or deny access based on authenticity of credentials.
            //

            IUserSecurityAuthority userAuthority = UserSecurityAuthority.FromConfig();
            object userToken = userAuthority.Authenticate(credentials.UserName,
                credentials.Password, null, SchemeName);

            if (userToken != null)
                Allow(context, new GenericIdentity(credentials.UserName, SchemeName), userToken);
            else
                Deny(context, credentials);
        }

        /// <summary>
        /// Retrieves the Basic credentials from the authorization request.
        /// </summary>

        protected virtual BasicCredentials GetCredentials(HttpContext context)
        {
            if (context == null)
                throw new ArgumentNullException("context");

            //
            // Get the authorization value from the request header and assert
            // that the request is indeed for Basic authentication. If not, then
            // just ignore it assuming that another registered module, which
            // knows how to handle it, will pick up the request.
            //

            string authorization = Mask.NullString(context.Request.Headers["Authorization"]).Trim();

            if (authorization.Length < _basicSpace.Length)
                return null;

            string scheme = authorization.Substring(0, _basicSpace.Length);

            if (!InvariantString.EqualsCaseless(scheme, _basicSpace))
                return null;

            //
            // Crack the basic-cookie, which is a base 64 encoded string of
            // the user's credentials.
            //

            string cookie = authorization.Substring(scheme.Length).TrimStart(' ');
            string credentialsString;

            try
            {
                credentialsString = CredentialsEncoding.GetString(Convert.FromBase64String(cookie));
            }
            catch (FormatException e)
            {
                throw new HttpException(400, "Bad Request", e);
            }

            //
            // The user name and password are separated by a colon (:) in the
            // credentials string, so split these up.
            //
            
            string[] credentials = credentialsString.Split(new char[] {':'}, 2);

            if (credentials.Length < 2)
                throw new HttpException(400, "Bad Request");

            string userName = credentials[0];
            string password = credentials[1];

            return new BasicCredentials(userName, password);
        }

        /// <summary>
        /// Issues the Basic challenge to the client.
        /// </summary>

        protected virtual void IssueChallenge(HttpContext context)
        {
            if (context == null)
                throw new ArgumentNullException("context");

            context.Response.AppendHeader("WWW-Authenticate", 
                "Basic Realm=\"" + GetRealm(context) + "\"");
        }

        /// <summary>
        /// Raises the Authenticate event and then allows access to the 
        /// authenticated identity by installing it as the user making the
        /// current request. 
        /// </summary>

        protected virtual void Allow(HttpContext context, IIdentity identity, object userToken)
        {
            //
            // Raise the Authenticate event. This gives the application the
            // chance to, for example, catch the event in Global.asax and
            // set a custom principal upon authentication.
            //

            BasicAuthenticationEventArgs eventArgs = new BasicAuthenticationEventArgs(context, identity, userToken);
            OnAuthenticate(eventArgs);
    
            //
            // If no user was set into the context by the event handler(s)
            // and a user object was supplied, then install it into the
            // context now.
            //
    
            if (context.User == null && eventArgs.User != null)
                context.User = eventArgs.User;
    
            //
            // No user set so far? Let's create a generic one with no roles.
            //
    
            if (context.User == null)
            {
                IPrincipal user = userToken as IPrincipal;
                    
                if (user == null)
                    user = new GenericPrincipal(eventArgs.Identity, null);
                    
                context.User = user;
            }
        }

        /// <summary>
        /// Emits an access denial respone to the client.
        /// </summary>

        protected virtual void Deny(HttpContext context, BasicCredentials credentials)
        {
            if (context == null)
                throw new ArgumentNullException("context");

            if (credentials == null)
                throw new ArgumentNullException("credentials");

            HttpResponse response = context.Response;
            response.StatusCode = 401;
            response.StatusDescription = "Access Denied";
            context.ApplicationInstance.CompleteRequest();
        }

        /// <summary>
        /// Raises the <see cref="Authenticate"/> event.
        /// </summary>

        protected virtual void OnAuthenticate(BasicAuthenticationEventArgs e)
        {
            if (e == null)
                throw new ArgumentNullException("e");

            BasicAuthenticationEventHandler handler = Authenticate;
            
            if (handler != null)
                handler(this, e);
        }

        /// <summary>
        /// Encoding used to convert Basic credentials from their base64
        /// representation into plain text.
        /// </summary>

        protected virtual Encoding CredentialsEncoding
        {
            get
            {
                //
                // IMPORTANT! 
                //
                // RFC2617 (HTTP Authentication: Basic and Digest Access
                // Authentication) does not seems to provide any details or
                // guidance on how the user name and password are encoded into
                // bytes before creating the base64 cookie. If the user agent
                // and the server are not in agreement over the byte encoding
                // then the credentials will fail to match albeit the user
                // having provided the right ones. For example, if user name is
                // "Müller" and the user agent uses the UTF-8 encoding while the
                // server assumes something else like Latin-1, then a mismatch
                // will result. Rather than make some unsafe assumption, this
                // property provides a way for a derived module to override the
                // encoding bias of this implementation. 
                //
                // This implementation assumes ISO 8859-1 (Latin-1) so it
                // won't be able to support some exotic cases. This choice does
                // nonetheless seem like the right bet becasue I found the
                // following in section 2.1.1 of RFC2831 (Using Digest
                // Authentication as a SASL Mechanism):
                //
                //    charset
                //       This directive, if present, specifies that the server
                //       supports UTF-8 encoding for the username and password.
                //       If not present, the username and password must be
                //       encoded in ISO 8859-1 (of which US-ASCII is a subset).
                //       The directive is needed for backwards compatibility
                //       with HTTP Digest, which only supports ISO 8859-1.
                //

                if (_credentialsEncoding == null)
                    _credentialsEncoding = Encoding.GetEncoding("iso-8859-1");

                return _credentialsEncoding;
            }
        }
    }
}
