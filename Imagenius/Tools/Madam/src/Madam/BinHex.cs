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
    /// Provides methods to format a buffer of bytes as a string containing a
    /// hex representation of each nibble and in the exact sequence in which
    /// they appear in the buffer.
    /// </summary>

    public sealed class BinHex
    {
        public static string Encode(byte[] buffer)
        {
            if (buffer == null)
                throw new ArgumentNullException("buffer");

            return Encode(buffer, 0, buffer.Length);
        }

        public static byte[] Decode(string buffer)
        {
            if (buffer == null)
                throw new ArgumentNullException("buffer");

            return Decode(buffer, 0, buffer.Length);
        }

        public static string Encode(byte[] buffer, int offset, int count)
        {
            return Encode(buffer, offset, count, false);
        }

        public static byte[] Decode(string buffer, int offset, int count)
        {
            return Decode(buffer, offset, count, false);
        }

        public static string Encode(byte[] buffer, int offset, int count, bool usingCaps)
        {
            if (buffer == null)
                throw new ArgumentNullException("buffer");

            if (offset < 0)
                throw new ArgumentOutOfRangeException("offset");

            if (count < 0)
                throw new ArgumentOutOfRangeException("count");

            int sourceIndex = offset;
            int sourceEndIndex = offset + count;

            if (sourceEndIndex > buffer.Length)
                throw new ArgumentOutOfRangeException("count");

            int targetIndex = 0;
            char[] chars = new char[count * 2];
            string hex = usingCaps ? "0123456789ABCDEF" : "0123456789abcdef";

            while (sourceIndex < sourceEndIndex)
            {
                byte b = buffer[sourceIndex++];
                chars[targetIndex++] = hex[b >> 4];
                chars[targetIndex++] = hex[b & 0xf];
            }

            return new string(chars);
        }

        public static byte[] Decode(string chars, int offset, int count, bool usingCaps)
        {
            if (chars == null)
                throw new ArgumentNullException("chars");

            if ((chars.Length % 2) != 0)
                throw new ArgumentOutOfRangeException("chars");

            if (offset < 0)
                throw new ArgumentOutOfRangeException("offset");

            if (count < 0)
                throw new ArgumentOutOfRangeException("count");

            int sourceIndex = offset;
            int sourceEndIndex = offset + count;

            if (sourceEndIndex > chars.Length)
                throw new ArgumentOutOfRangeException("count");

            int targetIndex = 0;
            byte[] buffer = new byte[count / 2];
            string hex = usingCaps ? "0123456789ABCDEF" : "0123456789abcdef";

            while (sourceIndex < sourceEndIndex)
            {
                // high-weight part
                int nIdx = hex.IndexOf(chars[sourceIndex++]);
                if ((nIdx < 0) || (nIdx > 15))
                    throw new ArgumentOutOfRangeException("chars");
                buffer[targetIndex] = (byte)(nIdx << 4);

                // low-weight part
                nIdx = hex.IndexOf(chars[sourceIndex++]);
                if ((nIdx < 0) || (nIdx > 15))
                    throw new ArgumentOutOfRangeException("chars");
                buffer[targetIndex++] += (byte)nIdx;
            }

            return buffer;
        }

        private BinHex()
        {
            throw new NotSupportedException();
        }
    }
}
