using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Runtime.Serialization;

namespace IGSMLib
{
    [DataContract]
    public class IGViewport
    {
        [DataMember(Name = "X", Order = 0)]
        float X;
        [DataMember(Name = "Y", Order = 1)]
        float Y;
        [DataMember(Name = "Z", Order = 2)]
        float Z;

        public readonly PointF m_ptCenter;
        public readonly double m_fZoom;
        public IGViewport(PointF ptCenter, double fZoom)
        {
            m_ptCenter = ptCenter;
            m_fZoom = fZoom;
        }
        public void GetParams()
        {
            X = m_ptCenter.X;
            Y = m_ptCenter.Y;
            Z = (float)m_fZoom;
        }
    }

    public class IGViewportMap
    {
        private Hashtable m_hashtable = null;

        public IGViewportMap()
        {
            m_hashtable = new Hashtable();
        }

        public IGViewport GetViewport (string sFrameId)
        {
            if (m_hashtable[sFrameId] == null)
                return null;
            return (IGViewport)m_hashtable[sFrameId];
        }

        public void SetViewport(string sFrameId, IGViewport newViewport)
        {
            m_hashtable[sFrameId] = newViewport;
        }

        public void Clear()
        {
            m_hashtable.Clear();
        }
    }
}
