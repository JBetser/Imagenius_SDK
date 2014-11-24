/*
 * Deep Zoom Batch Export Library
 * Created on 5/5/08
 * 
 * _______________________________________                
 * | Giorgio Sardo                         |              ___
 * | Microsoft User Experience Consultant  |              \__\_____/~~~\___ |
 * | giorgio.sardo@microsoft.com           |_______________\_______===== //}+
 * | http://blogs.msdn.com/Giorgio         |                 @'   ~~~| /~~~ |
 * |                                       |                         (+)
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 */
namespace DeepZoomLibrary
{
    using System;

    public class SparseImageToolNotFoundException : Exception
    {
        public override string Message
        {
            get
            {
                return "SparseImageTool not found";
            }
        }
    }

    public class MissingImagesException : Exception
    {
        public override string Message
        {
            get
            {
                return "The images collection is empty";
            }
        }
    }

    public class LoadImageException : Exception
    {
        public override string Message
        {
            get
            {
                return "Cannot read image file";
            }
        }
    }

    public class ImagesDestinationFolderException : Exception
    {
        public override string Message
        {
            get
            {
                return "Images Destination Folder error";
            }
        }
    }

    public class OutOfRangeException : Exception
    {
        public override string Message
        {
            get
            {
                return "The value specified is out of range";
            }
        }
    }
}
