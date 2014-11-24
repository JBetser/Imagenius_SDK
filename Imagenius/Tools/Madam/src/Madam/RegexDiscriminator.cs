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
    using System.Globalization;
    using System.Text.RegularExpressions;
    using System.Xml;

    #endregion

    /// <summary>
    /// A discriminator implementation that discriminates based on whether
    /// the result of an input expression evaluated against a context
    /// matches a regular expression pattern or not.
    /// </summary>

    public class RegexDiscriminator : IDiscriminator, IConfigurationCallback
    {
        private Regex _regex;
        private string _inputExpression;

        protected virtual Regex Expression
        {
            get { return _regex; }
        }

        public virtual string InputExpression
        {
            get { return Mask.NullString(_inputExpression); }
        }

        public virtual bool Qualifies(object context)
        {
            if (context == null)
                throw new ArgumentNullException("context");

            Regex expression = Expression;
            if (expression == null)
                return false;

            string input = EvaluateInput(context);
            return expression != null ? expression.Match(input).Success : false;
        }

        protected virtual string EvaluateInput(object context)
        {
            if (context == null)
                throw new ArgumentNullException("context");

            object result = DataExpressionEvaluator.Eval(context, InputExpression);
            return Convert.ToString(result, CultureInfo.InvariantCulture);
        }

        void IConfigurationCallback.Configure(XmlNode section)
        {
            Configure(section);
        }

        protected virtual void Configure(XmlNode section)
        {
            if (section == null)
                throw new ArgumentNullException("node");

            XmlAttributeCollection attributes = section.Attributes;

            string inputExpression = ConfigurationSectionHelper.GetValueAsString(attributes["inputExpression"]).Trim();
            string pattern = ConfigurationSectionHelper.GetValueAsString(attributes["pattern"]);

            Regex regex = null;
            
            if (pattern.Length > 0)
            {
                //
                // NOTE: There is an assumption here that most uses of this
                // discriminator will be for culture-insensitive matches. Since
                // it is difficult to imagine all the implications of involving
                // a culture at this point, it seems safer to just err with the
                // invariant culture settings.
                //

                RegexOptions options = RegexOptions.CultureInvariant;

                if (!ConfigurationSectionHelper.GetValueAsBoolean(attributes["caseSensitive"]))
                    options |= RegexOptions.IgnoreCase;

                if (!ConfigurationSectionHelper.GetValueAsBoolean(attributes["dontCompile"]))
                    options |= RegexOptions.Compiled;

                regex = new Regex(pattern, options);
            }

            //
            // Commit new state.
            //

            _inputExpression = inputExpression;
            _regex = regex;
        }
    }
}