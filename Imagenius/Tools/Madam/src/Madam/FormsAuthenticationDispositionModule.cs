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
    using System.Configuration;
    using System.Security;
    using System.Web;

    #endregion

    /// <summary>
    /// An ASP.NET module that determines the disposition of the Forms
    /// authentication module based on a configured set of criteria
    /// (discriminators). If the context of the running HTTP transaction
    /// matches the criteria then the Forms authentication module is muted
    /// and a standard unauthorized (401) response is sent, allowing an
    /// HTTP authentication scheme to take over.
    /// </summary>

    public class FormsAuthenticationDispositionModule : IHttpModule
    {
        private FormsAuthenticationDispositionConfiguration _config;

        public virtual void Init(HttpApplication context)
        {
            _config = (FormsAuthenticationDispositionConfiguration) ConfigurationSettings.GetConfig("madam/formsAuthenticationDisposition");
            context.EndRequest += new EventHandler(OnEndRequest);
        }

        public virtual void Dispose()
        {
        }

        private void OnEndRequest(object sender, EventArgs e)
        {
            OnEndRequest(((HttpApplication) sender).Context);
        }

        protected virtual void OnEndRequest(HttpContext context)
        {
            if (context == null)
                throw new ArgumentNullException("context");

            //
            // Is the response a redirection to a login page with a
            // post-authentication rendez-vous URL? If so, then we're seeing
            // Forms authentication in action. In that case, check if the
            // request qualifies to discriminate Forms. If it does, then
            // re-write an unauthorized (401) response as per the HTTP
            // specification.
            //
    
            HttpResponse response = context.Response;
    
            if (_config != null && 
                response.StatusCode == 302 && 
                _config.Discriminator.Qualifies(context))
            {
                RewriteUnauthorizedResponse(context);
            }
        }

        protected virtual void RewriteUnauthorizedResponse(HttpContext context)
        {
            if (context == null)
                throw new ArgumentNullException("context");

            HttpResponse response = context.Response;
    
            response.StatusCode = 401;
            response.StatusDescription = "Access Denied";

            //
            // Null out the Location header from the 302 response so that it
            // doesn't get transmitted to the client unnecessarily.
            //
            
            response.RedirectLocation = null;

            //
            // Change the response entity to reflect the right message. If we
            // don't clear out the response and re-write it here, then the user
            // might get the wrong message---that is, she might see an HTML page
            // stating that the resource has been moved (which would have been
            // right for 302) when what we want to indicate is an attempt to
            // access an unauthorized resource.
            //
    
            response.Clear();
            response.ContentType = "text/html";
            WriteUnauthorizedResponseHtml(context);
        }

        protected virtual void WriteUnauthorizedResponseHtml(HttpContext context)
        {
            //
            // Let's try to get the ASP.NET runtime to give us the HTML for a
            // security exception. If we can't get this, then we'll write out
            // our own default message. A derived implementation may want to
            // also want to involve custom error messages for production 
            // scenarios, which is not done here.
            //

            HttpException error = new HttpException(401, null, new SecurityException());
            string html = Mask.NullString(error.GetHtmlErrorMessage());

            if (html.Length == 0)
                html = DefaultUnauthorizedResponseHtml;

            context.Response.Write(html);
        }

        protected virtual string DefaultUnauthorizedResponseHtml
        {
            get
            {
                return @"
<html>
    <head>
        <title>401 Authorization Required</title>
    </head>
    <body>
        <h1>Authorization Required</h1>
        <p>This server could not verify that you are authorized to access the document
            requested.  Either you supplied the wrong credentials (e.g., bad password), or your
            browser doesn't understand how to supply the credentials required.</p>
    </body>
</html>";
            }
        }
    }
}