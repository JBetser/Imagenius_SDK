using System;
using System.Collections.Generic;
using System.Collections;
using System.Linq;
using System.Web;

namespace IGSMLib
{
    /// <summary>
    /// Summary description for InterThreadData
    /// </summary>
    public class InterThreadData<T>
    {
        private Object m_lockObject = null;
        private T m_data = default(T);

        public InterThreadData()
        {
            m_lockObject = new object();
        }

        public InterThreadData(T initValue)
        {
            m_lockObject = new object();
            m_data = initValue;
        }

        public void SetData(T newValue)
        {
            lock (m_lockObject)
            {
                m_data = newValue;
            }
        }

        public T GetData()
        {
            T copy = default(T);
            lock (m_lockObject)
            {
                copy = m_data;
            }
            return copy;
        }
    }
    /*
    public class SynchronizedCollection<T>
    {
        private Object m_lockObject = null;
        private List<T> m_dataList = null;

        public SynchronizedCollection()
        {
            m_lockObject = new object();
            m_dataList = new List<T>();
        }

        public SynchronizedCollection(List<T> initValue)
        {
            m_lockObject = new object();
            m_dataList = new List<T>();
            m_dataList.AddRange(initValue);
        }

        public void SetData(List<T> newValue)
        {
            lock (m_lockObject)
            {
                m_dataList.Clear();
                m_dataList.AddRange(newValue);
            }
        }

        public List<T> GetData()
        {
            List<T> copyDataList = new List<T>();
            lock (m_lockObject)
            {
                copyDataList.AddRange(m_dataList);
            }
            return copyDataList;
        }

        public void Add(T addValue)
        {
            lock (m_lockObject)
            {
                m_dataList.Add(addValue);
            }
        }

        public void Remove(T addValue)
        {
            lock (m_lockObject)
            {
                m_dataList.Remove(addValue);
            }
        }

        public int Length()
        {
            lock (m_lockObject)
            {
                return m_dataList.Count;
            }
        }

        public T Get(int nIndex)
        {
            lock (m_lockObject)
            {
                if ((nIndex >= 0) && (nIndex < m_dataList.Count))
                    return m_dataList[nIndex];
            }
            return default(T);
        }

        public void Clear()
        {
            lock (m_lockObject)
            {
                m_dataList.Clear();
            }
        }
    }*/

    public class InterThreadHashtable
    {
        private Object m_lockObject = null;
        private Hashtable m_hashtable = null;

        public InterThreadHashtable()
        {
            m_lockObject = new object();
            m_hashtable = new Hashtable();
        }

        public bool ContainsKey(object key)
        {
            lock (m_lockObject)
            {
                return m_hashtable.ContainsKey(key);
            }
        }

        public void Add(object key, object value)
        {
            lock (m_lockObject)
            {
                m_hashtable.Add(key, value);
            }
        }

        public void Remove(object key)
        {
            lock (m_lockObject)
            {
                m_hashtable.Remove(key);
            }
        }

        public object Get(object key)
        {
            lock (m_lockObject)
            {
                return m_hashtable[key];
            }
        }

        public void Set(object key, object value)
        {
            lock (m_lockObject)
            {
                m_hashtable[key] = value;
            }
        }

        public object GetKey(object value)
        {
            lock (m_lockObject)
            {
                IDictionaryEnumerator enumObjects = m_hashtable.GetEnumerator();
                while (enumObjects.MoveNext())
                {
                    if (enumObjects.Value == value)
                        return enumObjects.Key;
                }
                return null;
            }
        }

        public int Length()
        {
            lock (m_lockObject)
            {
                return m_hashtable.Count;
            }
        }

        public List<object> GetArrayKeys()
        {
            lock (m_lockObject)
            {
                List<object> copyList = new List<object>();
                IDictionaryEnumerator enumObjects = m_hashtable.GetEnumerator();
                while (enumObjects.MoveNext())
                    copyList.Add(enumObjects.Key);
                return copyList;
            }
        }

        public List<object> GetArrayValues()
        {
            lock (m_lockObject)
            {
                List<object> copyList = new List<object>();
                IDictionaryEnumerator enumObjects = m_hashtable.GetEnumerator();
                while (enumObjects.MoveNext())
                    copyList.Add(enumObjects.Value);
                return copyList;
            }
        }

        public Hashtable GetHashtable()
        {
            lock (m_lockObject)
            {
                Hashtable hashtable = new Hashtable();                
                IDictionaryEnumerator enumObjects = m_hashtable.GetEnumerator();
                while (enumObjects.MoveNext())
                    hashtable.Add(enumObjects.Key, enumObjects.Value);
                return hashtable;
            }
        }
    }
    
}
