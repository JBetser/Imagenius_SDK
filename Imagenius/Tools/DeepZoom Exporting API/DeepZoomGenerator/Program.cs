/*
 * Deep Zoom Batch Export Library
 * Created on 5/5/08
 *  - Updated support for DeepZoom Composer (Silverlight 2 Beta 2)
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
namespace DZTest
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Windows.Forms;
    using DeepZoomLibrary;

    class Program
    {

        static void Main(string[] args)
        {
            if (args.Length != 2)
            {
                Console.WriteLine("Wrong number of arguments : 2 (sourcePath destinationPath)");
                Console.ReadLine();
            }
            // Collection name
            string collectionName = "ImageniusDeepZoom";
            // Folder containing images to be processed
            string sourceImagePath = args[0];
            // Destination folder of the batch process
            string outputImagePath = args[1];

            CreateComposition(collectionName, sourceImagePath, outputImagePath);
            //CreateCollection(collectionName, sourceImagesFolder, outputFolder);
        }

        /// <summary>
        /// Create a Test composition using automation
        /// </summary>
        static void CreateComposition(string collectionName, string sourceImagePath, string outputImagePath)
        {
            // Create a collection converter
            DZIConverter compositionConverter = new DZIConverter();
            // Required parameters
            compositionConverter.SourcePath = sourceImagePath;
            compositionConverter.DestinationPath = outputImagePath;
            // Optional parameters
            compositionConverter.CollectionName = collectionName;
            compositionConverter.CompressionQuality = 1;
            //TODO: You can customize the exporting experience here, by setting the accordin parameters such as:
            // Tile Size, File Format, Compression, Quality, ...

            Console.WriteLine("Conversion started...");

            try
            {
                compositionConverter.BatchCollectionExport();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

            Console.WriteLine("Conversion completed\n");
        }

        /// <summary>
        /// Retrieve all the images in the specified folder
        /// </summary>
        /// <param name="directory"></param>
        /// <returns></returns>
        static IEnumerable<string> GetImages(string directory)
        {
            return from file in new DirectoryInfo(directory).GetFiles("*.jpg")
                   select file.FullName;
        }

        /// <summary>
        /// Return the SparseImageTool.exe path
        /// </summary>
        /// <param name="directory"></param>
        /// <returns></returns>
        static string GetSparseImageToolPath()
        {
            // Try to get the file from the default location
            string defaultPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles),
                "Microsoft\\Deep Zoom Composer\\SparseImageTool.exe");

            //If the file doesn't exist, it prompt the user to select it
            if (!File.Exists(defaultPath))
            {
                MessageBox.Show("In order to use this tool you need to have the Microsoft Deep Zoom Composer installed on your machine. Once you installed the tool, please select the SparseImageTool.exe assembly from the Deep Zoom installation folder.",
                    "Information",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Information);

                OpenFileDialog dialog = new OpenFileDialog();
                dialog.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles);
                dialog.Filter = "SparseImageTool.exe (*.exe)|*.exe|All files (*.*)|*.*";

                if (dialog.ShowDialog() == DialogResult.OK)
                {
                    if (Path.GetFileNameWithoutExtension(dialog.FileName).ToLower().Equals("sparseimagetool"))
                        defaultPath = dialog.FileName;
                    else
                        throw new SparseImageToolNotFoundException();
                }
                else
                    throw new SparseImageToolNotFoundException();
            }

            return defaultPath;
        }
    }
}