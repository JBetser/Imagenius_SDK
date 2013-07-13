#pragma once

/*----------      System and librairies headers     ----------*/ 
#include <windows.h>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <msxml2.h>

/*----------     Constant expressions definition    ----------*/ 


/*----------             Type definition            ----------*/ 

// KdtDescription:
// This is the abstract class to all format classes. It provides the 
// interface that suffices to define formats which are involved in data
// exchange mechanism.
// The main topic of this class and its derivatives is to manage the read/write
// of data from/into the OS resources. All the management of the format id is
// done here and supplies all the predefined ids.
// A method enables the user to register new formats and obtain a new id
// for it.@@ 
// An internal mechanism guarantees the unicity of ids used by an object.
// The objects of 'Format' class will be put on RogueWave container. So, the 
// 'Format' class needs to be derived from the class 'RWCollectable'.
class Format
{
  public:

	// KdtDescription:
	// Represents the possible return value when a client tries
	// to manipulate format.
	//
	// 'REGISTRATION_OK' : on success@@
	// 'NOTEXIST' : the format id is not an existing format. It has not been registered using the 'registerFormat'() method (Refer to the 'IGCommunicable' class for more information)@@
	// 'INVALID_PROTOCOL' : the action does not exist (either copy&paste or drag&drop)@@
	// 'INVALID_MODE' : the data transfer mode does not exist@@
	// 'UNEXPECTED_ERROR' : initialization problem@@
	// 'ALREADY_EXIST' : the request has already been done.
	enum retCode
	{
		REGISTRATION_OK = 0,		/// On success.
		NOTEXIST = 1,				/// The format id is not an existing format. It has not been registered using the 'registerFormat'() method (Refer to the 'IGCommunicable' class for more information).
		INVALID_PROTOCOL = 2,	/// The action does not exist (either copy&paste or drag&drop).
		INVALID_MODE = 3,			/// The data transfer mode does not exist.
		UNEXPECTED_ERROR = 4,	/// Initialization problem.
		ALREADY_EXIST = 6			/// The request has already been done.
	};

	// Lists the different format ids available.
	enum formatAvailable
	{
		F_NONE				= 0,					/// no format.
		F_TEXT				= CF_TEXT,				/// Text format.
		F_IGFRAME_BMP		= CF_BITMAP,			/// Device Dependent Bitmap format.
		F_IGFRAME_DIB		= CF_DIB,				/// Device Independent Bitmap format.
		F_IGFRAME			= CF_PRIVATEFIRST + 1,	/// IGFrameControl format.
		F_IGLAYER			= CF_PRIVATEFIRST + 2,	/// IGLayer format.
		F_IGSELECTION		= CF_PRIVATEFIRST + 3,	/// IGSelection format.
		F_END				= CF_PRIVATEFIRST + 4	/// First free id. When adding new format, this id is used to compute. The next available id.
	};

	// KdtDescription:
	// Lists the available mode of data transfer.
	//
	// 'MODE_IN' considers the mode of transfer as import,@@
	// 'MODE_OUT' considers the mode of transfer as export,@@
	// 'MODE_IN_OUT' considers the mode transfer as import/export.
	enum dataModeTransfert
	{
		MODE_IN = DATADIR_SET,	  /// Considers the mode of transfer as import.
		MODE_OUT = DATADIR_GET,   /// Considers the mode of transfer as export.
		MODE_IN_OUT = MODE_IN | MODE_OUT /// Considers the mode transfer as import/export.
	};

	// KdtDescription:
	// Default constructor. This constructor is inactive.
	Format();

	// KdtDescription:
	// Constructor. 'fid' is the id attributed to the format.
	Format(int fid);

	// KdtDescription:
	// Constructor. 'fid' is the id attributed to the format.
	// Constructor. 'sid' is the sytem id attributed to the format.
	Format(int fid, int sid);

	// KdtDescription:
	// Constructor. 'fid' is the id attributed to the format.
	// Constructor. 'szFormatName' is the name of the format, 
	// it's used to sytem generate then system id attributed to the format.
	Format(int fid, const std::wstring& sFormatName);

	// KdtDescription:
	// Constructor. 'szFormatName' is the name of the format, 
	// it's used to sytem generate then system id attributed to the format.
	Format(const std::wstring& sFormatName);

	// KdtDescription:
	// Virtual destructor. This destructor is inactive.
	virtual ~Format();

	// KdtDescription:
	// Writes data into an OS ressource.
	// Parameters:
	// 'ccontext' specifies the context of the current action.@@
	// 'is' is a stream where to extract the data.@@
	// 'hwnd' is the window handle of the client.@@
	// 'bufsize'is the size of the data contained in the stream.
	// Return:
	// 1 on success: the data has been correctly written into the OS ressource.@@
	// 0 on failure, a problem occurs during the writing process.@@
	// By default, returns 0.
	virtual int write(LPDATAOBJECT pIDataObject, std::istream& is, HWND hwnd, int bufsize = 0);

	// KdtDescription:
	// Reads data from an OS resource.
	// Parameters:
	// 'ccontext' specifies the context of the current action.@@
	// 'os' : a stream where to put the data.@@
	// 'hwnd' : the window of the client.
	// Return:
	// 1 on success: the data has been correctly written into the OS ressource.@@
	// 0 on failure, a problem occurs during the writing process.@@
	// By default, returns 0.
	virtual int read(LPDATAOBJECT pIDataObject, std::ostream& os, int* size, HWND hwnd);

	// KdtDescription:
	// Inspects the OS resource to check if a data is present.
	// 'x', 'y' are the drop coordinates. They are relevant only for 
	// drag and drop action.@@
	// Returns a value different from 0 if the data is found. 
	// Returns 0 by default.
	virtual bool isClipboardFormatAvailable() const;

	// KdtDescription:
	// Allows to know the complete name(human readable) of
	// the format in concern.@@
	// Returns a string corresponding to the complete name.
	// By default returns the string "Unknown".
	virtual std::wstring getName() const = 0;

	// KdtDescription:
	// Returns the id of the format.
	int getKobraID() const;

	// KdtDescription:
	// Returns a copy of the format object.
	// By default, does nothing.
	virtual Format* getCopy() const = 0;

	// KdtDescription:
	// Method specific to Windows NT platform.
	// Returns the id reserved in the OS for the format.
	// By default returns 0.
	UINT getSystemID() const;

	// KdtDescription:
	// Allows to guess if the format contains a 'KO_Data' or not.
	// This information is important for choosing the way the data is read.
	//
	// Returns 1 if the data is a 'KO_Data', 0 if not.
	// By default returns 0.
	virtual bool isKoData() const;

	static void RegisterFormats();
	static void UnregisterFormats();
	static std::list<Format*> GetFormats();

    static Format* findRegisteredFormat(int fid);

	static HRESULT DATAOBJECT2MSXML(const IDataObject&, unsigned long cfFormat, IXMLDOMDocument2**);

  private:
	// Description:
	// This method returns the next available Id, and ensures that
	// the id is unique.
	int allocateKobraID(const std::wstring& sFormatName);

  protected :
	// the kobra id
	int fid_;

	// the system id
	UINT sid_;

	//n a internal counter used to get the next id available for a new format registration
	static int counterType_; // never exported static
#pragma warning(push)
#pragma warning(disable: 4251)
	static std::list<Format*> defaultFormats_;
	static std::map<std::wstring, int> registeredFid_;
#pragma warning(pop)
};
