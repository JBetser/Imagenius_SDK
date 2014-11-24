<%@ WebHandler Class="MadamWeb.DownloadHandler" Language="C#" %>

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

namespace MadamWeb
{
    using System;
    using System.Collections;
    using System.Configuration;
    using System.Web;
    using System.IO;

    public class DownloadHandler : IHttpHandler
    {
        public void ProcessRequest(HttpContext context)
        {
            string downloadPath = context.Request.QueryString[null];

            if ((downloadPath == null || downloadPath.Length == 0) &&
                context.Request.PathInfo.Length != 0)
            {
                downloadPath = context.Request.PathInfo.Substring(1);
            }

            if (downloadPath == null)
                downloadPath = string.Empty;
                
            downloadPath = downloadPath.Trim();

            if (downloadPath.Length == 0)
                ThrowNotFound(null);
            
            string localDownloadPath = context.Server.MapPath("~/Downloads/" + Path.GetFileName(downloadPath));
            context.Response.ContentType = MimeMap.ContentTypeFromPath(localDownloadPath);
            context.Response.AppendHeader("Content-Disposition", "attachment; filename=" + Path.GetFileName(localDownloadPath));
            
            try
            {
                //
                // NOTE! HttpResponse.TransmitFile was added in a hotfix
                // after the release of Microsoft .NET Framework 1.x. If
                // you get compilation errors, you may not have the hotfix
                // applied and in which case you can use replace the call 
                // with HttpResponse.WriteFile below. For more information on
                // HttpResponse.TransmitFile for Microsoft .NET Framework
                // 1.x, see the following two Microsoft Knowledge Base 
                // articles:
                //
                // - FIX: The Aspnet_wp.exe process may stop responding when
                //   you use the Response.BinaryWrite method to download 
                //   large files from an ASP.NET page.
                //
                //   http://support.microsoft.com/kb/821387/en-us (1.0)
                //
                // - FIX: Downloading Large Files Causes a Large Memory Loss 
                //   and Causes the Aspnet_wp.exe Process to Recycle
                //
                //   http://support.microsoft.com/kb/823409/en-us (1.1)
                //
                
                context.Response.TransmitFile(localDownloadPath);
            }
            catch (FileNotFoundException e)
            {
                ThrowNotFound(e);
            }
        }

        public bool IsReusable
        {
            get { return true; }
        }

        private void ThrowNotFound(Exception innerException)
        {
            throw new HttpException(404, "Not Found", innerException);
        }

        private sealed class MimeMap
        {
            private static IDictionary _map;

            public const string ApplicationOctetStream = "application/octet-stream";

            static MimeMap()
            {
                _map = (IDictionary) ConfigurationSettings.GetConfig("madam/mimeMap");

                if (_map == null)
                    _map = CreateDefaultMap();
            }

            public static string ContentTypeFromPath(string path)
            {
                return ContentTypeFromExtension(Path.GetExtension(path));
            }

            public static string ContentTypeFromExtension(string extension)
            {
                extension = extension != null ? extension : string.Empty;
                
                if (extension.Length > 0)
                {
                    if (extension[0] != '.')
                        throw new ArgumentOutOfRangeException("extension");
                    
                    if (extension.Length == 1)
                        extension = string.Empty;
                }

                string contentType = (string) _map[extension];
                
                if (contentType == null || contentType.Length == 0)
                    contentType = MimeMap.ApplicationOctetStream;
                    
                return contentType;
            }

            #region Default MIME map

            private static Hashtable CreateDefaultMap()
            {
                Hashtable map = new Hashtable(/* capacity = */ 200,
                    CaseInsensitiveHashCodeProvider.DefaultInvariant,
                    CaseInsensitiveComparer.DefaultInvariant);
        
                map.Add(".323",     "text/h323");
                map.Add(".asx",     "video/x-ms-asf");
                map.Add(".acx",     "application/internet-property-stream");
                map.Add(".ai",      "application/postscript");
                map.Add(".aif",     "audio/x-aiff");
                map.Add(".aiff",    "audio/aiff");
                map.Add(".axs",     "application/olescript");
                map.Add(".aifc",    "audio/aiff");
                map.Add(".asr",     "video/x-ms-asf");
                map.Add(".avi",     "video/x-msvideo");
                map.Add(".asf",     "video/x-ms-asf");
                map.Add(".au",      "audio/basic");
                map.Add(".bin",     "application/octet-stream");
                map.Add(".bas",     "text/plain");
                map.Add(".bcpio",   "application/x-bcpio");
                map.Add(".bmp",     "image/bmp");
                map.Add(".cdf",     "application/x-cdf");
                map.Add(".cat",     "application/vndms-pkiseccat");
                map.Add(".crt",     "application/x-x509-ca-cert");
                map.Add(".c",       "text/plain");
                map.Add(".css",     "text/css");
                map.Add(".cer",     "application/x-x509-ca-cert");
                map.Add(".crl",     "application/pkix-crl");
                map.Add(".cmx",     "image/x-cmx");
                map.Add(".csh",     "application/x-csh");
                map.Add(".cod",     "image/cis-cod");
                map.Add(".cpio",    "application/x-cpio");
                map.Add(".clp",     "application/x-msclip");
                map.Add(".crd",     "application/x-mscardfile");
                map.Add(".dll",     "application/x-msdownload");
                map.Add(".dot",     "application/msword");
                map.Add(".doc",     "application/msword");
                map.Add(".dvi",     "application/x-dvi");
                map.Add(".dir",     "application/x-director");
                map.Add(".dxr",     "application/x-director");
                map.Add(".der",     "application/x-x509-ca-cert");
                map.Add(".dib",     "image/bmp");
                map.Add(".dcr",     "application/x-director");
                map.Add(".disco",   "text/xml");
                map.Add(".exe",     "application/octet-stream");
                map.Add(".etx",     "text/x-setext");
                map.Add(".evy",     "application/envoy");
                map.Add(".eml",     "message/rfc822");
                map.Add(".eps",     "application/postscript");
                map.Add(".flr",     "x-world/x-vrml");
                map.Add(".fif",     "application/fractals");
                map.Add(".gtar",    "application/x-gtar");
                map.Add(".gif",     "image/gif");
                map.Add(".gz",      "application/x-gzip");
                map.Add(".hta",     "application/hta");
                map.Add(".htc",     "text/x-component");
                map.Add(".htt",     "text/webviewhtml");
                map.Add(".h",       "text/plain");
                map.Add(".hdf",     "application/x-hdf");
                map.Add(".hlp",     "application/winhlp");
                map.Add(".html",    "text/html");
                map.Add(".htm",     "text/html");
                map.Add(".hqx",     "application/mac-binhex40");
                map.Add(".isp",     "application/x-internet-signup");
                map.Add(".iii",     "application/x-iphone");
                map.Add(".ief",     "image/ief");
                map.Add(".ivf",     "video/x-ivf");
                map.Add(".ins",     "application/x-internet-signup");
                map.Add(".ico",     "image/x-icon");
                map.Add(".jpg",     "image/jpeg");
                map.Add(".jfif",    "image/pjpeg");
                map.Add(".jpe",     "image/jpeg");
                map.Add(".jpeg",    "image/jpeg");
                map.Add(".js",      "application/x-javascript");
                map.Add(".lsx",     "video/x-la-asf");
                map.Add(".latex",   "application/x-latex");
                map.Add(".lsf",     "video/x-la-asf");
                map.Add(".mhtml",   "message/rfc822");
                map.Add(".mny",     "application/x-msmoney");
                map.Add(".mht",     "message/rfc822");
                map.Add(".mid",     "audio/mid");
                map.Add(".mpv2",    "video/mpeg");
                map.Add(".man",     "application/x-troff-man");
                map.Add(".mvb",     "application/x-msmediaview");
                map.Add(".mpeg",    "video/mpeg");
                map.Add(".m3u",     "audio/x-mpegurl");
                map.Add(".mdb",     "application/x-msaccess");
                map.Add(".mpp",     "application/vnd.ms-project");
                map.Add(".m1v",     "video/mpeg");
                map.Add(".mpa",     "video/mpeg");
                map.Add(".me",      "application/x-troff-me");
                map.Add(".m13",     "application/x-msmediaview");
                map.Add(".movie",   "video/x-sgi-movie");
                map.Add(".m14",     "application/x-msmediaview");
                map.Add(".mpe",     "video/mpeg");
                map.Add(".mp2",     "video/mpeg");
                map.Add(".mov",     "video/quicktime");
                map.Add(".mp3",     "audio/mpeg");
                map.Add(".mpg",     "video/mpeg");
                map.Add(".ms",      "application/x-troff-ms");
                map.Add(".nc",      "application/x-netcdf");
                map.Add(".nws",     "message/rfc822");
                map.Add(".oda",     "application/oda");
                map.Add(".ods",     "application/oleobject");
                map.Add(".pmc",     "application/x-perfmon");
                map.Add(".p7r",     "application/x-pkcs7-certreqresp");
                map.Add(".p7b",     "application/x-pkcs7-certificates");
                map.Add(".p7s",     "application/pkcs7-signature");
                map.Add(".pmw",     "application/x-perfmon");
                map.Add(".ps",      "application/postscript");
                map.Add(".p7c",     "application/pkcs7-mime");
                map.Add(".pbm",     "image/x-portable-bitmap");
                map.Add(".ppm",     "image/x-portable-pixmap");
                map.Add(".pub",     "application/x-mspublisher");
                map.Add(".pnm",     "image/x-portable-anymap");
                map.Add(".pml",     "application/x-perfmon");
                map.Add(".p10",     "application/pkcs10");
                map.Add(".pfx",     "application/x-pkcs12");
                map.Add(".p12",     "application/x-pkcs12");
                map.Add(".pdf",     "application/pdf");
                map.Add(".pps",     "application/vnd.ms-powerpoint");
                map.Add(".p7m",     "application/pkcs7-mime");
                map.Add(".pko",     "application/vndms-pkipko");
                map.Add(".ppt",     "application/vnd.ms-powerpoint");
                map.Add(".pmr",     "application/x-perfmon");
                map.Add(".pma",     "application/x-perfmon");
                map.Add(".pot",     "application/vnd.ms-powerpoint");
                map.Add(".prf",     "application/pics-rules");
                map.Add(".pgm",     "image/x-portable-graymap");
                map.Add(".qt",      "video/quicktime");
                map.Add(".ra",      "audio/x-pn-realaudio");
                map.Add(".rgb",     "image/x-rgb");
                map.Add(".ram",     "audio/x-pn-realaudio");
                map.Add(".rmi",     "audio/mid");
                map.Add(".ras",     "image/x-cmu-raster");
                map.Add(".roff",    "application/x-troff");
                map.Add(".rtf",     "application/rtf");
                map.Add(".rtx",     "text/richtext");
                map.Add(".sv4crc",  "application/x-sv4crc");
                map.Add(".spc",     "application/x-pkcs7-certificates");
                map.Add(".setreg",  "application/set-registration-initiation");
                map.Add(".snd",     "audio/basic");
                map.Add(".stl",     "application/vndms-pkistl");
                map.Add(".setpay",  "application/set-payment-initiation");
                map.Add(".stm",     "text/html");
                map.Add(".shar",    "application/x-shar");
                map.Add(".sh",      "application/x-sh");
                map.Add(".sit",     "application/x-stuffit");
                map.Add(".spl",     "application/futuresplash");
                map.Add(".sct",     "text/scriptlet");
                map.Add(".scd",     "application/x-msschedule");
                map.Add(".sst",     "application/vndms-pkicertstore");
                map.Add(".src",     "application/x-wais-source");
                map.Add(".sv4cpio", "application/x-sv4cpio");
                map.Add(".tex",     "application/x-tex");
                map.Add(".tgz",     "application/x-compressed");
                map.Add(".t",       "application/x-troff");
                map.Add(".tar",     "application/x-tar");
                map.Add(".tr",      "application/x-troff");
                map.Add(".tif",     "image/tiff");
                map.Add(".txt",     "text/plain");
                map.Add(".texinfo", "application/x-texinfo");
                map.Add(".trm",     "application/x-msterminal");
                map.Add(".tiff",    "image/tiff");
                map.Add(".tcl",     "application/x-tcl");
                map.Add(".texi",    "application/x-texinfo");
                map.Add(".tsv",     "text/tab-separated-values");
                map.Add(".ustar",   "application/x-ustar");
                map.Add(".uls",     "text/iuls");
                map.Add(".vcf",     "text/x-vcard");
                map.Add(".wps",     "application/vnd.ms-works");
                map.Add(".wav",     "audio/wav");
                map.Add(".wrz",     "x-world/x-vrml");
                map.Add(".wri",     "application/x-mswrite");
                map.Add(".wks",     "application/vnd.ms-works");
                map.Add(".wmf",     "application/x-msmetafile");
                map.Add(".wcm",     "application/vnd.ms-works");
                map.Add(".wrl",     "x-world/x-vrml");
                map.Add(".wdb",     "application/vnd.ms-works");
                map.Add(".wsdl",    "text/xml");
                map.Add(".xml",     "text/xml");
                map.Add(".xlm",     "application/vnd.ms-excel");
                map.Add(".xaf",     "x-world/x-vrml");
                map.Add(".xla",     "application/vnd.ms-excel");
                map.Add(".xls",     "application/vnd.ms-excel");
                map.Add(".xof",     "x-world/x-vrml");
                map.Add(".xlt",     "application/vnd.ms-excel");
                map.Add(".xlc",     "application/vnd.ms-excel");
                map.Add(".xsl",     "text/xml");
                map.Add(".xbm",     "image/x-xbitmap");
                map.Add(".xlw",     "application/vnd.ms-excel");
                map.Add(".xpm",     "image/x-xpixmap");
                map.Add(".xwd",     "image/x-xwindowdump");
                map.Add(".xsd",     "text/xml");
                map.Add(".z",       "application/x-compress");
                map.Add(".zip",     "application/x-zip-compressed");
                map.Add(".*",       "application/octet-stream");

                //
                // NOTE! If you add more MIME mappings here, do not forget to
                // update the capacity of the hashtable.
                //

                return map;
            }

            #endregion
        }
    }
}
