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
    using System.Diagnostics;
    using System.IO;
    using Microsoft.DeepZoomTools;
    using System.Windows.Media;
    using System.Windows.Forms;
    using System.Collections.Generic;

    /// <summary>
    /// Creates a DZI Image from the nodes in the scene graph.
    /// </summary>
    public class DZIConverter
    {
        #region Enums

        public enum TileSizes
        {
            Normal,
            Large
        }

        public enum FileFormats
        {
            TIF,
            PNG,
            JPG
        }

        #endregion

        #region Constants

        private const string argument = "CreateDeepZoomImage";

        #endregion

        #region Properties

        /// <summary>
        /// Images (full path) collection to convert.
        /// </summary>
        public string SourcePath { get; set; }

        /// <summary>
        /// Destination folder where to save the exported collection.
        /// </summary>
        public string DestinationPath { get; set; }

        /// <summary>
        /// Collection name.
        /// </summary>
        public string CollectionName { get; set; }

        /// <summary>
        /// Specifies the size of each tile.
        /// </summary>
        private TileSizes TileSizeValue = TileSizes.Normal;
        public TileSizes TileSize
        {
            get { return TileSizeValue; }
            set { TileSizeValue = value; }
        }

        /// <summary>
        /// Specifies the number of overlap pixels in each tile.
        /// Valid overlap values are between 0 and 10. 
        /// Default image tile overlap is 1 pixel.
        /// </summary>
        private int OverlapPixelsValue = 1;
        public int OverlapPixels
        {
            get { return OverlapPixelsValue; }
            set
            {
                if (value > 10 || value < 0)
                    throw new OutOfRangeException();
                OverlapPixelsValue = value;
            }
        }

        /// <summary>
        /// Specifies the compression quality of JPEG tiles. 
        /// Valid quality values are from 1 to 100. 
        /// Default quality is 95.
        /// </summary>
        private int CompressionQualityValue = 95;
        public int CompressionQuality
        {
            get { return CompressionQualityValue; }
            set
            {
                if (value > 100 || value < 1)
                    throw new OutOfRangeException();
                CompressionQualityValue = value;
            }
        }

        #endregion

        #region Methods

        /// <summary>
        /// Start the batch process. You need to specify the source images folder and destination images folder before starting this process.
        /// </summary>
        public void BatchCollectionExport()
        {
            // Check parameters
            if (string.IsNullOrEmpty(DestinationPath) || !Directory.Exists(DestinationPath.Substring(0, DestinationPath.LastIndexOf("\\"))))
                throw new ImagesDestinationFolderException();

            if (string.IsNullOrEmpty(CollectionName))
                CollectionName = "BatchCollection";

            SparseImageCreator dziCreator = new SparseImageCreator();
            dziCreator.BackgroundColor = Color.FromRgb(255, 255, 255);
            dziCreator.ConversionImageQuality = CompressionQuality;
            dziCreator.ImageQuality = CompressionQuality;
            dziCreator.ConversionTileSize = (TileSize == TileSizes.Normal) ? 254 : 508;
            dziCreator.TileSize = (TileSize == TileSizes.Normal) ? 254 : 508;
            dziCreator.ConversionTileOverlap = OverlapPixels;
            dziCreator.TileOverlap = OverlapPixels;
            dziCreator.TileFormat = ImageFormat.Jpg;

            if ((SourcePath == null) || (!File.Exists(SourcePath)))
                throw new LoadImageException();
            Image im = new Image(SourcePath);
            if (im == null)
                throw new LoadImageException();
            ICollection<Image> imageCollection = new List<Image>();
            imageCollection.Add(im);
            dziCreator.Create(imageCollection, DestinationPath);
        }

        #endregion
    }
}
