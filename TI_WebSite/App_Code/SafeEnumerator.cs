using System;
using System.Collections.Generic;

namespace IGPE
{
    /// <summary>
    /// Summary description for SafeEnumerator
    /// </summary>
    public static class SafeEnumerator
    {
        public static IEnumerable<T> GetSafeishEnumerator<T>(this SynchronizedCollection<T> sc)
        {
            if (object.ReferenceEquals(null, sc)) yield break;
            lock (sc.SyncRoot)
            {
                if (0 >= sc.Count) yield break; 
                foreach (var i in sc)
                {
                    yield return i;
                }
            }
        }
    }
}
