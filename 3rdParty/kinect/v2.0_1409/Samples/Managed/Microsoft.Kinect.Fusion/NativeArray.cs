// -----------------------------------------------------------------------
// <copyright file="NativeArray.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>
// -----------------------------------------------------------------------

namespace Microsoft.Kinect.Fusion
{
    using System;
    using System.Collections.Generic;
    using System.Runtime.InteropServices;

    /// <summary>
    /// The NativeArray provides the ability to access native array data.
    /// </summary>
    /// <typeparam name="T">The data type stores in the native array.</typeparam>
    internal class NativeArray<T> : IList<T>
    {
        private IntPtr ptr;
        private int count;
        private int elementSize;

        public NativeArray(IntPtr ptr, int count)
        {
            this.ptr = ptr;
            this.count = count;

            elementSize = Marshal.SizeOf(typeof(T));
        }

        public int Count
        {
            get { return count; }
        }

        public bool IsReadOnly
        {
            get { return true; }
        }

        public T this[int index]
        {
            get
            {
                if (index >= count)
                {
                    throw new ArgumentOutOfRangeException("index");
                }

                return (T)Marshal.PtrToStructure(IntPtr.Add(ptr, index * elementSize), typeof(T));
            }

            set
            {
                throw new NotSupportedException();
            }
        }

        public int IndexOf(T item)
        {
            unsafe
            {
                for (int i = 0; i < count; i++)
                {
                    if (this[i].Equals(item))
                    {
                        return i;
                    }
                }
            }

            return -1;
        }

        public bool Contains(T item)
        {
            return IndexOf(item) != -1;
        }

        public void CopyTo(T[] array, int arrayIndex)
        {
            if (null == array)
            {
                throw new ArgumentNullException("array");
            }

            if (Count > array.Length - arrayIndex)
            {
                throw new ArgumentOutOfRangeException("arrayIndex");
            }

            for (int i = 0; i < Count; i++)
            {
                array[arrayIndex + i] = this[i];
            }
        }

        public IEnumerator<T> GetEnumerator()
        {
            for (int i = 0; i < count; ++i)
            {
                yield return this[i];
            }
        }

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        #region Not Supported Methods

        public void Insert(int index, T item)
        {
            throw new NotSupportedException();
        }

        public void RemoveAt(int index)
        {
            throw new NotSupportedException();
        }

        public bool Remove(T item)
        {
            throw new NotSupportedException();
        }

        public void Add(T item)
        {
            throw new NotSupportedException();
        }

        public void Clear()
        {
            throw new NotSupportedException();
        }

        #endregion
    }
}
