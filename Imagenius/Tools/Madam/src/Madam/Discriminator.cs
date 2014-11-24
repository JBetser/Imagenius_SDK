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
    using System.Configuration;
    using System.Xml;

    #endregion

    /// <summary>
    /// Provides discrimination based on evaluation of contained/child
    /// discriminators.
    /// </summary>

    public class Discriminator : IDiscriminator, IConfigurationCallback
    {
        private bool _all;
        private DiscriminatorCollection _children = DiscriminatorCollection.Empty;

        public virtual bool Qualifies(object context)
        {
            if (context == null)
                throw new ArgumentNullException("context");

            if (!HasDiscriminators)
                return false;

            //
            // Walk through all child discriminators and determine the
            // outcome, OR-ing or AND-ing each qualification as needed.
            //

            bool result = false;

            foreach (IDiscriminator discriminator in Discriminators)
            {
                if (discriminator != null)
                {
                    if (discriminator.Qualifies(context))
                    {
                        if (!_all) return true;
                        result = true;
                    }
                    else
                    {
                        if (_all) return false;
                    }
                }
            }

            return result;
        }

        public bool HasDiscriminators
        {
            get { return _children != null && _children.Count > 0; }
        }

        public DiscriminatorCollection Discriminators
        {
            get { return _children; }
        }

        void IConfigurationCallback.Configure(XmlNode section)
        {
            Configure(section);
        }

        protected virtual void Configure(XmlNode section)
        {
            if (section == null)
                throw new ArgumentNullException("node");

            bool all = ConfigurationSectionHelper.GetValueAsBoolean(section.Attributes["all"]);
    
            ArrayList discriminatorList = null;
    
            foreach (XmlNode childNode in section.ChildNodes)
            {
                //
                // Skip comments and whitespaces.
                //

                if (childNode.NodeType == XmlNodeType.Comment ||
                    childNode.NodeType == XmlNodeType.Whitespace)
                {
                    continue;
                }

                //
                // Node must be an element named <discriminator>.
                //

                if (childNode.NodeType != XmlNodeType.Element)
                {
                    throw new ConfigurationException(
                        string.Format("Unexpected type of node ({0}) in configuration.", childNode.NodeType.ToString()), 
                        childNode);
                }

                if (childNode.Name != "discriminator")
                {
                    throw new ConfigurationException(
                        string.Format("'{0}' is not a valid discriminator node.", childNode.Name), 
                        childNode);
                }

                //
                // Get the type specification of the discriminator that will be
                // used subsequently to create an instance of it.
                //

                string typeSpec = string.Empty;
                XmlAttribute typeAttribute = (XmlAttribute) childNode.Attributes.RemoveNamedItem("type");
                
                if (typeAttribute != null)
                    typeSpec = ConfigurationSectionHelper.GetValueAsString(typeAttribute);

                if (typeSpec.Length == 0)
                    throw new ConfigurationException("Missing discriminator type specification.", childNode);
                
                //
                // Create an instance of the discriminator.
                //

                Type type = Type.GetType(typeSpec, true);
                object o = Activator.CreateInstance(type);
                IDiscriminator discriminator = o as IDiscriminator;

                if (discriminator == null)
                    throw new ConfigurationException(string.Format("{0} is not a valid discriminator object.", o.GetType().FullName), childNode);

                //
                // Does it expect to be called back at configuration-time? If
                // yes, then send the discriminator the current node so that it
                // can use it to load its configuration.
                //
                
                IConfigurationCallback configCallback = discriminator as IConfigurationCallback;
                
                if (configCallback != null)
                    configCallback.Configure(childNode);

                //
                // Add the discriminator to the end of the list, dynamically
                // allocating the list on first use. In general, we don't expect
                // a lot of discriminators for most cases so we initialize the
                // list with a modest capacity for 5 items.
                //

                if (discriminatorList == null)
                    discriminatorList = new ArrayList(5);

                discriminatorList.Add(discriminator);
            }

            //
            // Create a strong-typed collection of discriminators from the
            // ArrayList.
            //
    
            DiscriminatorCollection children = null;
    
            if (discriminatorList != null)
                children = new DiscriminatorCollection(discriminatorList);

            //
            // All has gone well if we made it this far, so commit the new
            // state.
            //
    
            _all = all;
            _children = children;
        }
    }
}