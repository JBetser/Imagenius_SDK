// kocommunicable.h
#pragma once

#include "Format.h"

/*----------      System and librairies headers     ----------*/ 
#include <list>
#include <strstream>
#include <iostream>

class FormatCommunication;
class IGCommunicable;
/*----------     Constant expressions definition    ----------*/ 


/*----------             Type definition            ----------*/ 

// The class IGDropSource allows the IGDataObject to be transported
// via OLE drag and drop.
class IGDropSource: public IDropSource
{
public:
	STDMETHODIMP QueryInterface (REFIID riid, LPVOID * ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	STDMETHODIMP QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);
	STDMETHODIMP GiveFeedback(DWORD dwEffect);

	IGDropSource(IGCommunicable* pKO_Communicable);

private:
	~IGDropSource();

private:
	ULONG             m_refs;
	IGCommunicable*  m_pCommunicable;
};

// The class IGDropTarget allows to receive data from a IDataObject
// via OLE drag and drop
class IGDropTarget : public IDropTarget
{
public:
	STDMETHODIMP QueryInterface (REFIID riid, LPVOID * ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	STDMETHODIMP DragEnter(LPDATAOBJECT pDataObject, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
	STDMETHODIMP DragOver(DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
	STDMETHODIMP DragLeave();
	STDMETHODIMP Drop(LPDATAOBJECT pDataObject, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);

	IGDropTarget(IGCommunicable* pKO_Communicable, HWND hWnd);

	HWND getHWND() const
	{
		return m_hWnd;
	}

private:
	bool matchFormat(LPDATAOBJECT pDataObject) const;
	~IGDropTarget();

private:
	IGCommunicable	*m_pCommunicable;
	HWND             m_hWnd;
	ULONG            m_refs;
	LPDATAOBJECT     m_pDataObject;
};

// KdtDescription:
// This is the abstract class for communicable objects. It provides
// methods for performing data exchange between objects. It contains
// a part of the scontroller object which controls both Copy&Paste
// to clipboard and Drag&Drop operations. It may contain several
// controllers if it uses the multiple drop site capabilities.
class IGCommunicable
{
public:
	// KdtDescription:
	// Represents the possible return values when a client
	// tries to initialize the controller.
	enum retCode
	{
		CONTROLLER_OK = 0,		/// Controller initialization OK.
			CONTROLLER_ERROR = 1		/// Controller initialization ERROR.
	};
	
	// KdtDescription:
	// Represents the possible state of the controller during
	// a drag&drop operation:@@
	// 'noDragDrop' : no drag and drop in progress.@@
	// 'readyToDrag' : a drag is possibly going to start.@@
	// 'isDragging' : a drag is in progress.
	enum controllerState
	{
		noDragDrop,		/// no drag and drop in progress.
			readyToDrag,	/// a drag is possibly going to start.
			isDragging		/// a drag is in progress.
	};
	
	// KdtDescription:
	// Default constructor.
	// Performs initializations.
	IGCommunicable();
	
	// KdtDescription:
	// Virtual destructor.
	// Releases the set of previously created controllers.
	virtual ~IGCommunicable();
	
	bool preProcessingInput(MSG &msg);
	
	// KdtDescription:
	// Initializes the object.@@
	// First empties the list of controllers and initializes
	// the set with a controller dedicated to the given window 'hwnd'.
	// 'hwnd' is the window handle of the object which actually
	// contains the data.@@
	// This call must appear once inside an object.@@
	// An object may be composed of several windows. In such a
	// case, call the method 'addController'() for each additional window.
	// Return:
	//  - 'CONTROLLER_OK' if the initialization succeeds,@@
	//  - 'CONTROLLER_ERROR' if not.@@
	// IMPORTANT: Call this method before invoking 'registerFormat'() method.
	int initializeController(HWND hwnd);
	int initializeController(HWND hwnd, IGDropTarget* pDropTarget);
	
	// KdtDescription:
	// Declares a new drop site for the given window 'hwnd'.
	// Return:
	//  - 'CONTROLLER_OK' if the initialization succeeds,@@
	//  - 'CONTROLLER_ERROR' if not.@@
	// IMPORTANT: 
	// Call this method before invoking 'registerFormat'() method.
	int addController(HWND hwnd);
	
	// KdtDescription:
	// Removes a drop site recognised by its given window handle
	// 'hwnd', from the list of controllers.
	// Return:
	//  - 'CONTROLLER_OK' if the initialization succeeds.@@
	//  - 'CONTROLLER_ERROR' if not.
	int removeController(HWND hwnd);
	
	// KdtDescription:
	// Registers a format that the controller will manage during
	// any data exchange mechanism.
	// Parameters:
	// 'fid' : the id of the format (among those already existing).@@
	// 'cid' : the operation to perform.@@
	// 'mode' : indicates if the format is used for importation
	//        or exportation or both (the default value).@@
	// 'first' : sets this format as the default one for importation. 
	//         Give a value other than 0 in order to render 
	//         this format the default one.
	//
	// Return: 
	// 'REGISTRATION_OK' on success.@@
	// 'NOTEXIST' if the given 'fid' is not an existing format. It has not 
	// been registered using the 'registerFormat'() method.@@
	// 'INVALID_PROTOCOL' if the given 'cid' does not exist.@@
	// 'INVALID_MODE' if the given 'mode' does not exist.@@
	// 'ALREADY_EXIST' if the request has already be done.@@
	// 'UNEXPECTED_ERROR' else (the controller is not correctly initialized).@@
	// Refer to the 'Format' class for more information.
	// IMPORTANT: Call this method after all call to 'initializeController'()
	//            have been performed and refer to the "KDT Companion Guide"
	//          for more information.
	int registerFormat(int fid, int cid, int mode = Format::MODE_IN_OUT, bool bFirst = false);
	
	// Description:
	// Removes the format among the list of registered formats
	//
	// Parameters:
	// fid: format identifier
	// cid: data transfer protocol (D&D or C&P)
	// mode: data transfer mode (in,out or both)
	// Return: 
	// 'REGISTRATION_OK' on success.@@
	// 'NOTEXIST' if the given 'fid' is not an existing format. It has not 
	int unregisterFormat(int fid, int cid, int mode);
	
	// KdtDescription:
	// Declares a new format.
	// Parameters:
	// 'fmt' : an object of class 'Format'.@@
	// 'ret_type' : contains the id of this new format. This id is to
	//            be used later when performing 'registerFormat'().@@
	// Refer to the 'registerFormat'() method for return value possibilities.
	int registerNewFormat(Format* fmt, int* ret_type);
	
	// KdtDescription:
	// Allows you to modify the default format with 'fid'.
	// Return:
	// 'REGISTRATION_OK' on success.@@
	// 'NOTEXIST' if the given 'fid' is not an existing format. It has not 
	// been registered using the 'registerFormat'() method.@@
	// Refer to the 'Format' class for more information.
	int setFormatAsDefault(int fid);
	
	// Description:
	// This method returns the currently available formats.
	// The criterions of availability is : 
	//             - the mode of transfer is valid (in or out)
	//             - the format has been registered for this action
	// 
	// Parameters:
	// liste : return the available formats in a Rogue Wave liste 
	// mode : the mode of transfer, i-e importation or exportation
	// cid : the current user action, i-e drag&drop or copy/paste 
	// 
	// Return:
	// the number of available formats
	virtual int getFormatAvailable(std::list<Format*>& liste, int mode, int cid) const;
	
	// KdtDescription:
	// Allows you to make it possible to drag and drop inside the 
	// same object.
	// If the object does not use this method, the default behaviour
	// will prevent from inside drag and drop.
	void acceptInnerDragDrop(bool b = true);
	
	// KdtDescription:
	// Invokes a copy to clipboard operation.
	// Enumerates every concerned format, then copies this data
	// to the clipboard, according to the format.@@
	bool copyToClipboard();
	
	// KdtDescription:
	// Calls a paste from the clipboard operation.
	// Enumerates every format, starting with the default one, until
	// it encounters the same format in the clipboard.
	bool pasteFromClipboard();
	
	// KdtDescription:
	// Calls a paste special from a clipboard operation.
	// The user is asked for a format to use for the paste,
	// using a dialogue box. A 'root' window (usually the main
	// window of the application) is needed to create the dialogue
	// box.@@
	// If the clipboard does not contain the requested format then no paste is 
	// achieved.@@
	// Returns the format fid actually pasted from the clipboard.
	//bool pasteSpecialFromClipboard(HWND root);
	
	// KdtDescription:
	// Starts a Drag&Drop operation.
	// Checks if a Drag&Drop operation is actually possible and if
	// so, it prepares the controller to drag.@@
	// Note that the Drag&Drop has not been fully started yet.
	// Parameters:
	// 'win' : the window where the drag occurs.@@
	// 'x', 'y' : the drag position inside the 'VWindow'.
	virtual void startDrag(HWND hWnd, int x, int y);
	virtual void startDrag(MSG& msg);
	
	// KdtDescription:
	// Continues a Drag&Drop operation.
	// Actually starts the Drag&Drop operation.
	// Parameters:
	// 'win' : the window where to continue the drag action.@@
	// 'x', 'y' : the current position inside the 'VWindow'.
	virtual void continueDrag(HWND hWnd, int x, int y);
	virtual void continueDrag(MSG& msg);
	
	// KdtDescription:
	// Ends a drag&drop operation.
	// Parameters:
	// 'win' : the window where the drag ends.@@
	// 'x', 'y' : the current position inside the 'VWindow'.
	virtual void endDrag(HWND hWnd, int x, int y);
	virtual void endDrag(MSG& msg);
	
	int getDragDropDelta() const;
	
	// KdtDescription:
	// Obtains the state of the controller through Drag&Drop.
	// Return:
	// 'noDragDrop' when no Drag&Drop is initiated.@@
	// 'readyToDrag' after initiation of a Drag&Drop.@@
	// 'isDragging' when a Drag&Drop operation is in progress.
	int dragDropState();
	
	// KdtDescription:
	// Allows an object to put data on a stream, according to a format.
	// Parameters:
	// 'vos' : the stream where to place the data.@@
	// 'fid' : the id of the format.@@
	// 'cid' : the current action (either 'DRAG_DROP_ID' or 'COPY_PASTE_ID').@@
	// 'x', 'y' : the drag coordinates relatively to the window.@@
	// 'hwnd' : the window where the drag starts.@@
	// Returns 1 on success, 0 if not. By default behaviour, returns 0.
	virtual int exportData(std::ostream& vos, int fid, int cid, int x, int y, HWND hwnd);
	
	// KdtDescription:
	// Allows an object to get data from a stream, according to a format.
	//
	// Returns 1 on success, 0 if not. By default behaviour, returns 0.
	//
	// Parameters:
	// 'vis' : the stream from where to retrieve the data.@@
	// 'fid' : the id of the format.@@
	// 'bufsize' : the size of the data to read from the stream.@@
	// 'cid' : the current action (either 'DRAG_DROP_ID' or 'COPY_PASTE_ID').@@
	// 'x', 'y' : the drop coordinates relatively to the window.@@
	// 'hwnd' : the window where the drop occurs.
	virtual int importData(std::istream& vis, int fid, int bufsize, int cid, int x, int y, HWND hwnd);
	
	// KdtDescription:
	// Allows an object, according to a format, to get data from a stream.
	//
	// Returns 1 on success, 0 if not. By default behaviour, returns 0.
	//
	// Parameters:
	// 'vis' : the stream from where to retrieve the data.@@
	// 'fid' : the id of the format.@@
	// 'bufsize' : the size of the data to read from the stream.@@
	// 'cid' : the current action (either 'DRAG_DROP_ID' or 'COPY_PASTE_ID').@@
	// 'x', 'y' : the drop coordinates relatively to the window.@@
	// 'hwnd' : the window where the drop occurs.@@
	// 'isAKoData' : 1 if the stream contains a 'KO_Data', 0 if not
	virtual int importData(std::istream& vis, int fid, int bufsize, int cid, int x, int y, HWND hwnd, int isAKoData);
	
	// KdtDescription:
	// Allows an object, according to a format, to get data from a stream.
	//
	// Returns 1 on success, 0 if not. By default behaviour, returns 0.
	//
	// Parameters:
	// 'vis' : the stream from where to retrieve the data.@@
	// 'fid' : the id of the format.@@
	// 'bufsize' : the size of the data to read from the stream.@@
	// 'cid' : the current action (either 'DRAG_DROP_ID' or 'COPY_PASTE_ID').@@
	// 'x', 'y' : the drop coordinates relatively to the window.@@
	// 'hwnd' : the window where the drop occurs.@@
	// 'isAKoData' : 1 if the stream contains a 'KO_Data', 0 if not@@
	// 'isDragInitiator' : 1 if the drag process was initiated by the same object. This parameter is relevant only when the 
	//                     cid is equal to DRAG_DROP_ID (ie during a drop action)
	virtual int importData(std::istream& vis, int fid, int bufsize, int cid, int x, int y, HWND hwnd, int isAKoData, int isDragInitiator);
	
	// KdtDescription:
	// Allows an object to perform separate actions when a drag
	// starts.
	// Parameters:
	// 'win' : the window where the drag starts.@@
	// 'x', 'y' : the drag position in 'win'.@@
	// Returns 1 if the drag action can start,
	// 0 if not. By default returns 1.
	virtual int dragDropIsStarting(HWND win, int x, int y);
	
	///Define if Drag&Drop is allowed
	virtual bool isDragAllowed(int x, int y);

	// KdtDescription:
	// Notifies the end of the drag&drop operation@@
	// Both drag initiator and drop target are notified of the
	// occurence of the drop.
	// IMPORTANT: when implementing this method, do not forget to
	// call the base class method.
	virtual void dropComplete();
	
	// Description:
	// Compute the cursor effect depending on the current keystate, the position of the mouse.
	//
	// Return:
	// 0 if the operation succeeds
	// a <0 value trigger an error
	//
	// Default behaviour :
	// This method return 0, and does not modify the pdwEffect
	virtual HRESULT setCursorEffect(int grfKeyState, int x, int y, int* pdwEffect);
	
	// Description:
	// Allows object to accept drop action to occur.
	//
	// Return:
	// 1 if the drop is allowed
	// 0 otherwise
	//
	// Default behaviour :
	// This method return 1, so every drop action is accepted
	virtual bool acceptDropAction() const;
	
	bool getAcceptInnerDragDrop() const;
	
	// KdtDescription:
	// Lists the possible user action:@@
	// 'DRAG_DROP_ID' is a Drag&Drop user action.@@
	// 'COPY_PASTE_ID' is a Copy&Paste clipboard user action.
	enum userAction
	{
		DRAG_DROP_ID = 1,		/// Drag&Drop user action.
			COPY_PASTE_ID = 2		/// Copy&Paste clipboard user action.
	};
	
	// Description:
	// Sets the default datafeed used within the application
	static void setDefaultDatafeed(char datafeed);
	
	// Description:
	// Returns the default datafeed used within the application
	static char getDefaultDatafeed();
	
	// Indicates whether the object has triggered the D&D
	// The parameter 'dragId' will be compare with the internal counter
	bool isDragInitiator() const;
	
	// KdtDescription:
	// Allows an object to indicate whether a paste action is possible or not by
	// inspecting the clipboard.
	// By default, it inspects the clipboard to know which formats it contains.
	// 'mask' represents all currently available formats among
	// those registered by the object.
	// The object can derivate this method to perform additionnal checks.
	// Returns 0 if no format is found, 
	// 1 if at least one format exists.
	bool isClipboardFormatAvailable(int fid = Format::F_NONE) const;
	
	// Description:
	// This method informs if there is a format to drag
	//
	// Parameters:
    // cid : the kind of action
	//
	// Return:
	// 1 if there is
	// 0 if not
	bool existClipboardFormat(int fid = Format::F_NONE) const;
	
	// Description:
	// This method informs if there is a format to drag
	//
	// Parameters:
    // cid : the kind of action
	//
	// Return:
	// 1 if there is
	// 0 if not
	bool existDragDropFormat(int fid = Format::F_NONE) const;
	
	// Description:
	// This method informs if there is a format to drag
	//
	// Parameters:
    // cid : the kind of action
	//
	// Return:
	// 1 if there is
	// 0 if not
	bool existExportFormat(int fid, int cid) const;
	
	// Description:
	// Returns 1 if the format is already registered
	//
	// Parameters:
	// fid : the format id
	// cid : the kind of action
	// mode : the transfer mode
	// 
	// Return:
	// 1 if the format was found
	// 0 if not found
	bool isFormatRegistered(int fid, int cid, int mode) const; 

//	ostrstream os;
//  ...
//  IGCommunicable::CopyToClipboard(os, Format::...);
//  os.freeze(0);
	static bool CopyToClipboard(std::ostrstream& vos, int fid);

	// Description:
	// Checks if the import format "loop" can be executed or not
	virtual bool	importFormatAllowed(LPDATAOBJECT lpDataObject, int cid, HWND hWnd, int x, int y, int theFid) const;

	// Description:
	// This method actually starts the drag&drop operation.
	//
	// Parameters:
	// x,y : the current position inside the VWindow
	// 
	// Return:
	bool initiateDragAndDrop(HWND hWnd, int x, int y);

	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

protected:
	// Description:
	// This method transfers data from the OS resources to the client object. 
	// In fact, it finds the matching format and send the data to the client
	// object.
	//
	// Parameters:
	// cid : the kind of action
	// theFid : a fid format in order to force the importation of this particular format
	bool set(LPDATAOBJECT m_lpDataObject, int cid, HWND hWnd = 0, int x = 0, int y = 0, int theFid = Format::F_NONE);
	virtual bool preProcessingImportData (LPDATAOBJECT m_lpDataObject);
	virtual bool endExport (LPDATAOBJECT lpDataObject) const;
	Format* findRegisteredFormat(int fid) const;
	void freeDropTargets();

private:
	int							m_refs;
	// Description:
	// This method creates a dialog with the user for the selection
	// of a format.
	//
	// Parameters:
	// root : the parent of the dialog window
	// liste : a Rogue Wave list containing the formats
	// num : the number of formats
	// 
	// Return: 
	// the rank of the selected format
	// -1 on error
	//Format* pasteSpecialDialog(HWND root, std::list<Format*>& liste, LPDATAOBJECT pIDataObject) const;
	
	// Description:
	// Create a DataObject for a Copy/Paste or a Drag and Drop.
	bool queryDataObject(LPDATAOBJECT* pIDataObject, int cid);
	
	// Description:
	// This method resets and prepares the clipboard before putting data on it
	//
	// Parameters:
	// mode : clear the clipboard if different from 0
	//
	// Return:
	// 1 on success
	// 0 on failure, for example if the clipboard is locked.
	bool initiateClipboard(LPDATAOBJECT *pIDataObject, int mode);
	
	// Description:
	// This method removes the lock from the clipboard
	//
	// Parameters:
	// mode : Remove the lock on the clipboard depending on the kind of 
	// action, either copy or paste.
	//
	// Return:
	// 1 on success
	// 0 on failure, for example if the clipboard is locked.
	bool terminateClipboard(LPDATAOBJECT pIDataObject, int mode);
	
	bool importFormat(LPDATAOBJECT lpDataObject, Format* fmt, int cid, HWND hWnd, int x, int y);
	bool exportFormat(LPDATAOBJECT lpDataObject, Format* fmt, int cid, HWND hWnd, int x, int y);
	
	bool defineObjectDescriptor(LPDATAOBJECT pIDataObject) const;
	
	FormatCommunication* findFormatCommunication(int fid) const;
	
	IGDropTarget* findDropTarget(HWND hwnd) const;
	
	// Description:
	// Set the current Drag and Drop Manager
	static void setDragDropController(IGCommunicable*);
	
	// Description:
	// Get the current Drag and Drop Manager
	static IGCommunicable* getDragDropController();
	
protected :
	// Description:
	// This method transfers data from the client object to
	// the OS resources. In fact, it lists all the registered formats
	// and writes them.
	// 
	// Parameters:
	// cid : the kind of action
	// x,y : coordinates inside the window where the data is obtained
	//       only relevant for drag&drop
	bool get(LPDATAOBJECT m_lpDataObject, int cid, HWND hWnd = 0, int x = 0, int y = 0);

	// Accept inner drag and drop, ie the object can drop into itself.
	// Default value is FALSE;
	bool allowInnerDragAndDrop_;
	
	// This list contains the declared controller.
	// A controller is dedicated to a particular window inside the object.
	// Each window is considered as a valid drop zone.
#pragma warning(push)
#pragma warning(disable: 4251)
	std::list<IGDropTarget*> dropTargetsList_;
	
	std::list<Format*> registeredFormats_;
	std::list<FormatCommunication*> communicationFormats_;
#pragma warning(pop)
private :
	// These members are reserved for D&D action. It remembers the controller concerned
	// by the current drag and drop.
	static IGCommunicable* pDDCommunicable_;
	static int sourceX_;
	static int sourceY_;
	static char defaultDatafeed_;
	
	friend class IGDropTarget;
};
