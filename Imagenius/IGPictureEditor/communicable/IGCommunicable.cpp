#include "stdafx.h"
#include "IGCommunicable.h"
#include "IGDataObject.h"

#include "FormatComm.h"

#include <windowsx.h>
#include <minmax.h>
#include <oledlg.h>
#include <vector>


// static
int IGCommunicable::sourceX_ = -1;
int IGCommunicable::sourceY_ = -1;
IGCommunicable* IGCommunicable::pDDCommunicable_ = 0;
char IGCommunicable::defaultDatafeed_ = 'Q';


IGCommunicable::IGCommunicable()
: allowInnerDragAndDrop_(false)
, m_refs (1)
{
	// default formats
	registeredFormats_ = Format::GetFormats();
}

IGCommunicable::~IGCommunicable()
{
	//if (pDDCommunicable_ = this)
	// celcerce 10/05/2006 (REQ 16281 - Improve Drag&Drop) == Bug correction!
	if (pDDCommunicable_ == this)
	{
		pDDCommunicable_ = 0;
	}

	registeredFormats_.clear();

	while(!communicationFormats_.empty())
	{
		delete communicationFormats_.front();
		communicationFormats_.pop_front();
	}

	freeDropTargets();
}

// static
IGCommunicable* IGCommunicable::getDragDropController()
{
	return pDDCommunicable_;
}

// static
void IGCommunicable::setDefaultDatafeed(char datafeed)
{
  defaultDatafeed_ = datafeed;
}

// static
char IGCommunicable::getDefaultDatafeed()
{
  return defaultDatafeed_;
  //  return 'Q';
}

bool IGCommunicable::isDragInitiator() const
{
	return (pDDCommunicable_ == this);
}

// virtual
void IGCommunicable::dropComplete()
{
}

IGDropTarget* IGCommunicable::findDropTarget(HWND hwnd) const
{
	std::list<IGDropTarget*>::const_iterator iter = dropTargetsList_.begin();
	while (iter != dropTargetsList_.end())
    {
		if (hwnd == (*iter)->getHWND())
		{
			return (*iter);
		}
		++iter;
    }
	return 0;
}

void IGCommunicable::startDrag(HWND hWnd, int x, int y)
{
	IGCommunicable::setDragDropController(0);

	IGDropTarget* pDropTarget = findDropTarget(hWnd);
	if (!pDropTarget || !existDragDropFormat())
	{
		return;
	}
	sourceX_ = x;
	sourceY_ = y;
	if (!dragDropIsStarting (hWnd, sourceX_, sourceY_))
	{
		return;
	}
	IGCommunicable::setDragDropController(this);
}

void IGCommunicable::startDrag(MSG& msg)
{
	startDrag(msg.hwnd, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
}

void IGCommunicable::continueDrag(HWND hWnd, int x, int y)
{
	if (!IGCommunicable::getDragDropController())
	{
		return;
	}
	IGDropTarget* pDropTarget = findDropTarget(hWnd);
	if (!pDropTarget)
	{
		IGCommunicable::setDragDropController(0);
		return;
	}
	int cxDrag = ::GetSystemMetrics(SM_CXDRAG);
	int cyDrag = ::GetSystemMetrics(SM_CYDRAG);
	if ((abs(sourceX_ - x) > cxDrag) || (abs(sourceY_ - y) > cyDrag))
	{
		initiateDragAndDrop(hWnd, sourceX_, sourceY_);
		IGCommunicable::setDragDropController(0);
	}
}

void IGCommunicable::continueDrag(MSG& msg)
{
	continueDrag(msg.hwnd, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
}

void IGCommunicable::endDrag(HWND /*hWnd*/, int /*x*/, int /*y*/)
{
	IGCommunicable::setDragDropController(0);
}

void IGCommunicable::endDrag(MSG& /*msg*/)
{
	IGCommunicable::setDragDropController(0);
}

int IGCommunicable::dragDropState()
{
	if (IGCommunicable::getDragDropController())
	{
		return readyToDrag;
	}
	else
	{
		return noDragDrop;
	}
}

bool IGCommunicable::isDragAllowed(int /*x*/, int /*y*/)
{
	return true;
}

bool IGCommunicable::preProcessingInput(MSG& msg)
{
	if (msg.message == WM_LBUTTONDOWN)
	{
		int x = GET_X_LPARAM(msg.lParam);
		int y = GET_Y_LPARAM(msg.lParam);

		//to check if we are above a area
		//where drag&drop is allowed
		if(isDragAllowed(x,y))
     	   startDrag(msg);
    }
	else if (msg.message == WM_MOUSEMOVE)
	{
		if (::GetKeyState(VK_LBUTTON) < 0)
		{
			continueDrag(msg);
		}
	}
	else if (msg.message == WM_LBUTTONUP)
	{
		endDrag(msg);
    }
	return false;
}

int IGCommunicable::exportData(std::ostream& /*os*/, int, int, int, int, HWND)
{
	return 0;
}

int IGCommunicable::importData(std::istream& is, int fid, int bufsize, int cid, int x, int y, HWND hwnd, int isAKoData, int)
{
	return importData(is, fid, bufsize, cid, x, y, hwnd, isAKoData);
}

int IGCommunicable::importData(std::istream& is, int fid, int bufsize, int cid, int x, int y, HWND hwnd, int)
{
	return importData(is, fid, bufsize, cid, x, y, hwnd);
}

int IGCommunicable::importData(std::istream& /*is*/, int, int, int, int, int, HWND)
{
	return 0;
}

int IGCommunicable::dragDropIsStarting(HWND, int, int)
{
	// By default, a drag action is always allowed anywhere inside the window
	return 1;
}

int IGCommunicable::initializeController(HWND hwnd)
{
	// Free the list of controllers
	freeDropTargets();
	return addController(hwnd);
}

int IGCommunicable::initializeController(HWND hwnd, IGDropTarget* pDropTarget)
{
	// Free the list of controllers
	freeDropTargets();
	if (hwnd && pDropTarget)
    {
		dropTargetsList_.push_back (pDropTarget);
		::RegisterDragDrop (hwnd, pDropTarget);
		return CONTROLLER_OK;
    }
	return CONTROLLER_ERROR;
}

int IGCommunicable::addController(HWND hwnd)
{
	if (findDropTarget(hwnd))
	{
		return CONTROLLER_OK;
    }
	IGDropTarget* pDropTarget = new IGDropTarget(this, hwnd);
	if (pDropTarget != 0)
    {
		dropTargetsList_.push_back(pDropTarget);
		::RegisterDragDrop(hwnd, pDropTarget);
		return CONTROLLER_OK;
    }
	else
	{
		return CONTROLLER_ERROR;
	}
}

int IGCommunicable::removeController(HWND hwnd)
{
	std::list<IGDropTarget*>::iterator iter = dropTargetsList_.begin();
	while (iter != dropTargetsList_.end())
    {
		IGDropTarget* pDropTarget = (*iter);
		if (pDropTarget->getHWND() == hwnd)
		{
// PB 1
			if (::IsWindow(pDropTarget->getHWND()))
			{
				::RevokeDragDrop(pDropTarget->getHWND());
			}
			pDropTarget->Release();
			dropTargetsList_.erase(iter);
			return CONTROLLER_OK;
		}
		++iter;
	}
	return CONTROLLER_ERROR;
}

void IGCommunicable::freeDropTargets()
{
	std::list<IGDropTarget*>::const_iterator iter = dropTargetsList_.begin();
	while (iter != dropTargetsList_.end())
    {
// PB 1
// Sometimes, it's too late and the Revoke doesn't call Release on pDropTarget because the hWnd
// is no more valid
		IGDropTarget* pDropTarget = (*iter);
		if (::IsWindow(pDropTarget->getHWND()))
		{
			::RevokeDragDrop(pDropTarget->getHWND());
		}
//		pDropTarget->Release();
		++iter;
	}
	dropTargetsList_.clear();
}

int IGCommunicable::getDragDropDelta() const
{
	int cxDrag = ::GetSystemMetrics(SM_CXDRAG);
	int cyDrag = ::GetSystemMetrics(SM_CYDRAG);
	return max(cxDrag, cyDrag);
}

// static
void IGCommunicable::setDragDropController(IGCommunicable* pCommunicable)
{
	pDDCommunicable_ = pCommunicable;
}

HRESULT IGCommunicable::setCursorEffect(int /*grfKeyState*/, int /*x*/, int /*y*/, int* pdwEffect)
{
	if (pdwEffect)
		*pdwEffect=DROPEFFECT_COPY;
	return S_OK;
}

bool IGCommunicable::acceptDropAction() const
{
	return true;
}

void IGCommunicable::acceptInnerDragDrop(bool b)
{
	allowInnerDragAndDrop_ = b;
}

bool IGCommunicable::getAcceptInnerDragDrop() const
{
	return allowInnerDragAndDrop_;
}

Format* IGCommunicable::findRegisteredFormat(int fid) const
{
	Format* pFormat = Format::findRegisteredFormat(fid);
	if (!pFormat)
	{
		std::list<Format*>::const_iterator iter = registeredFormats_.begin();
		while(iter != registeredFormats_.end())
		{
			if ((*iter)->getKobraID() == fid)
			{
				pFormat = (*iter);
				break;
			}
			++iter;
		}
	}
	return pFormat;
}

FormatCommunication* IGCommunicable::findFormatCommunication(int fid) const
{
	std::list<FormatCommunication*>::const_iterator iter = communicationFormats_.begin();
	while(iter != communicationFormats_.end())
	{
		if ((*iter)->getKobraID() == fid)
		{
			return (*iter);
		}
		++iter;
	}
	return 0;
}

int IGCommunicable::registerFormat(int fid, int cid, int mode, bool bFirst /*= false*/)
{
	if (!(cid & IGCommunicable::COPY_PASTE_ID) && !(cid & IGCommunicable::DRAG_DROP_ID))
    {
		return Format::INVALID_PROTOCOL;
    }
	if (!(mode & DATADIR_SET) && !(mode & DATADIR_GET))
    {
		return Format::INVALID_MODE;
    }

	FormatCommunication* pFormatComm = findFormatCommunication(fid);
	if (pFormatComm && (pFormatComm->cid() & cid) && (pFormatComm->mode() & mode))
    {
		return Format::REGISTRATION_OK;
    }

	if (pFormatComm)
	{
		pFormatComm->cid(pFormatComm->cid() | cid);
		pFormatComm->mode(pFormatComm->mode() | mode);
		if (bFirst && (communicationFormats_.front() != pFormatComm))
		{
			communicationFormats_.remove(pFormatComm);
			communicationFormats_.push_front(pFormatComm);
		}
	}
	else
	{
		Format* pFormat= findRegisteredFormat(fid);
		if (!pFormat)
		{
			return Format::NOTEXIST;
		}
		if (bFirst)
		{
			// This format is to be considered first. We add it at the beginning
			// the list.
			communicationFormats_.push_front(new FormatCommunication(pFormat, cid, mode));
		}
		else
		{ // else, we just add it at the end of the list
			communicationFormats_.push_back(new FormatCommunication(pFormat, cid, mode));
		}
	}
	return Format::REGISTRATION_OK;
}

int IGCommunicable::registerNewFormat(Format* fmt, int* ret_type)
{
	*ret_type = fmt->getKobraID();
	Format* newFmt_ = fmt->getCopy();
	registeredFormats_.push_back(newFmt_);
	return Format::REGISTRATION_OK;
}

bool IGCommunicable::isFormatRegistered(int fid, int cid, int mode) const
{
	FormatCommunication* pFormatComm = findFormatCommunication(fid);
	return (pFormatComm &&
		 (pFormatComm->cid() & cid) &&
		 (pFormatComm->mode() & mode)
		);
}

int IGCommunicable::unregisterFormat(int fid, int cid, int mode)
{
	FormatCommunication* pFormatComm = findFormatCommunication(fid);
	if (!pFormatComm)
	{
		return Format::NOTEXIST;
	}

	pFormatComm->cid(pFormatComm->cid() & ~cid);
	pFormatComm->mode(pFormatComm->mode() & ~mode);

	if ((pFormatComm->cid() == 0) || (pFormatComm->mode() == 0))
	{
		communicationFormats_.remove(pFormatComm);
		delete pFormatComm;
	}
	return Format::REGISTRATION_OK;
}

int IGCommunicable::getFormatAvailable(std::list<Format*>& liste, int mode, int cid) const
{
	int nb = 0;
	std::list<FormatCommunication*>::const_iterator iter = communicationFormats_.begin();
	while(iter != communicationFormats_.end())
	{
		FormatCommunication* pFormatComm = (*iter);
		if ((pFormatComm->mode() & mode) &&
			(pFormatComm->cid() & cid))
		{
			liste.push_back(pFormatComm->format());
			++nb;
		}
		++iter;
	}
	return nb;
}

int IGCommunicable::setFormatAsDefault(int fid)
{
	// This method place the format fid at the beginning of the list.
	// The one registered for Drag&Drop as well as the one registered for copy/paste are concerned.
	int nResult = Format::NOTEXIST;
	FormatCommunication* pFormatCom = findFormatCommunication(fid);
	if (pFormatCom)
	{
		nResult = Format::REGISTRATION_OK;
		if (communicationFormats_.front() != pFormatCom)
		{
			communicationFormats_.remove(pFormatCom);
			communicationFormats_.push_front(pFormatCom);
		}
	}
	return nResult;
}

bool IGCommunicable::exportFormat (LPDATAOBJECT lpDataObject, Format* fmt, int cid, HWND hWnd, int x, int y)
{
	bool bResult = false;
	std::strstreambuf streambuffer(255);
	streambuffer.pubsetbuf (0, 255);
	std::ostream os(&streambuffer);
	int fid = fmt->getKobraID();
	if (exportData(os, fid, cid, x, y, hWnd))
	{
		os<<std::ends;
		std::istream is (&streambuffer);
		bResult = fmt->write (lpDataObject, is, hWnd, (int)streambuffer.pcount()) ? true : false;
	}
	streambuffer.freeze(0);
	return bResult;
}

bool IGCommunicable::get(LPDATAOBJECT lpDataObject, int cid, HWND hWnd, int x, int y)
{
	std::list<Format*> liste_formats;
	getFormatAvailable(liste_formats, DATADIR_GET, cid);

	bool bResult = false;
	std::list<Format*>::iterator iter = liste_formats.begin();

	while(iter != liste_formats.end())
	{
		Format* fmt = (*iter);
		if (exportFormat (lpDataObject, fmt, cid, hWnd, x, y))
		{
			bResult = true;
		}
		++iter;
    }

	endExport (lpDataObject);

	return bResult;
}

bool IGCommunicable::importFormat(LPDATAOBJECT lpDataObject, Format* fmt, int cid, HWND hWnd, int x, int y)
{
	bool bResult = false;
	int size;
	std::strstreambuf streambuffer(255);
	streambuffer.pubsetbuf(0, 255);
	std::ostream os(&streambuffer);
	int fid = fmt->getKobraID();    
	if (fmt->read (lpDataObject, os, &size, hWnd))
	{		
		std::istream is (&streambuffer);
        bResult = importData (is, fid, size, cid, x, y, hWnd, fmt->isKoData(), isDragInitiator()) ? true : false;
	}
	streambuffer.freeze(0);
	return bResult;
}

bool IGCommunicable::preProcessingImportData(LPDATAOBJECT lpDataObject)
{
	return false;
}

bool IGCommunicable::set(LPDATAOBJECT lpDataObject, int cid, HWND hWnd, int x, int y, int theFid)
{
	// the editor may want to manage this incoming data
	if (preProcessingImportData(lpDataObject))
	{
		return true;
	}
	if(importFormatAllowed(lpDataObject, cid, hWnd, x, y, theFid))
	{
		if (theFid == 0)
	    {
			// we try the default format, then the other, and stops when match
			std::list<Format*> liste_formats;
			getFormatAvailable (liste_formats, DATADIR_SET, cid);

			std::list<Format*>::iterator iter = liste_formats.begin();
			while(iter != liste_formats.end())
			{
				Format* fmt = (*iter);
				if (importFormat(lpDataObject, fmt, cid, hWnd, x, y))
				{
					// we find the first match, the enumeration std::ends.
					return true;
				}
				++iter;
			}
	    }
		else  // we use a special format
	    {
			FormatCommunication* pFormatComm = findFormatCommunication(theFid);
			if (pFormatComm &&
				(pFormatComm->cid() & cid) &&
				(pFormatComm->mode() & DATADIR_SET)
				)
			{
				Format* fmt = pFormatComm ->format();
				if (importFormat(lpDataObject, fmt, cid, hWnd, x, y))
				{
					// we find the first match, the enumeration std::ends.
					return true;
				}
			}
	    }
	 }
	return false;
}

bool IGCommunicable::defineObjectDescriptor(LPDATAOBJECT /*pIDataObject*/) const
{
	return false;
/*
	if (!pIDataObject)
	{
		return false;
	}
	std::string sFullUserTypeName = "Kobra 4.5";

	// Get the length of Full User Type Name; Add 1 for the null terminator
	DWORD dwFullUserTypeNameLen = sFullUserTypeName.size() + 1;

    // No src moniker so use user type name as source string.
    char* lpszSrcOfCopy =  (char*) sFullUserTypeName.c_str();
    DWORD dwSrcOfCopyLen = dwFullUserTypeNameLen;

	// Allocate space for OBJECTDESCRIPTOR and the additional string data
	DWORD dwObjectDescSize = sizeof(OBJECTDESCRIPTOR);
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT, dwObjectDescSize + (dwFullUserTypeNameLen + dwSrcOfCopyLen) * sizeof(char));
	if (!hMem)
	{
		return false;
	}

	LPOBJECTDESCRIPTOR lpOD = (LPOBJECTDESCRIPTOR)GlobalLock(hMem);

	// Set the FullUserTypeName offset and copy the string
	if (dwFullUserTypeNameLen)
	{
		lpOD->dwFullUserTypeName = dwObjectDescSize;
		strcpy((char*)((LPBYTE)lpOD+lpOD->dwFullUserTypeName), sFullUserTypeName.c_str());
	}
	else
	{
		lpOD->dwFullUserTypeName = 0;  // zero offset indicates that string is not present
	}
	// Set the SrcOfCopy offset and copy the string
	if (lpszSrcOfCopy)
	{
		lpOD->dwSrcOfCopy = dwObjectDescSize + dwFullUserTypeNameLen * sizeof(char);
		strcpy((char*)((LPBYTE)lpOD+lpOD->dwSrcOfCopy), lpszSrcOfCopy);
	}
	else
	{
		lpOD->dwSrcOfCopy = 0;  // zero offset indicates that string is not present
	}

	// Initialize the rest of the OBJECTDESCRIPTOR
	lpOD->cbSize       = dwObjectDescSize + (dwFullUserTypeNameLen + dwSrcOfCopyLen) * sizeof(char);
	lpOD->clsid        = CLSID_NULL;
//	lpOD->dwDrawAspect = 0;
//	lpOD->sizel        = {0, 0};
//	lpOD->pointl       = {0, 0};
//	lpOD->dwStatus     = 0;

	GlobalUnlock(hMem);

	// Call to SetData
	FORMATETC f;
	memset(&f, 0, sizeof(f));

	STGMEDIUM m;

	f.cfFormat = ::RegisterClipboardFormat(CF_OBJECTDESCRIPTOR);
	f.tymed = TYMED_HGLOBAL;
	m.hGlobal = hMem;
	m.pUnkForRelease = 0;
	return SUCCEEDED(pIDataObject->SetData(&f, &m, TRUE)) ? true : false;
*/
}

bool IGCommunicable::queryDataObject(LPDATAOBJECT* ppIDataObject, int cid)
{
	*ppIDataObject = 0;
	IGDataObject* pCDataObject = new IGDataObject();
	HRESULT hr = pCDataObject->QueryInterface(IID_IDataObject, (void **) ppIDataObject);
	pCDataObject->Release();
#ifdef DEBUG
	if (FAILED(hr))
	{
		::MessageBoxW(NULL, L"Cannot get the IDataObject Object", L"", MB_OK);
	}
#endif
	if (SUCCEEDED(hr))
	{
		defineObjectDescriptor(*ppIDataObject);
		return true;
	}
	else
	{
		return false;
	}
}

bool IGCommunicable::initiateDragAndDrop(HWND hWnd, int x, int y)
{
	LPDATAOBJECT pIDataObject = 0;
	if (!queryDataObject(&pIDataObject, IGCommunicable::DRAG_DROP_ID))
	{
		return false;
	}

	get(pIDataObject, IGCommunicable::DRAG_DROP_ID, hWnd, x, y);

	IGDropSource* pDropSource = new IGDropSource(this);

	DWORD effect;
	// GV 5/4/2005 DROPEFFECT_MOVE|DROPEFFECT_COPY|DROPEFFECT_LINK indicates that move, copy or link operation are available
	// The drop target will choose which operation is necessary
	::DoDragDrop (pIDataObject, pDropSource, DROPEFFECT_MOVE|DROPEFFECT_COPY|DROPEFFECT_LINK, &effect);

	dropComplete();
	pIDataObject->Release();
	pDropSource->Release();

	return true;
}

// Initiate the system clipboard
// the parameter 'empty' indicates that the clipboard needs to be cleared
bool IGCommunicable::initiateClipboard(LPDATAOBJECT* ppIDataObject, int mode)
{
	if (mode == DATADIR_GET)
	{
		return queryDataObject(ppIDataObject, IGCommunicable::COPY_PASTE_ID);
	}
	else
	{
		HRESULT hr = OleGetClipboard(ppIDataObject);
		return SUCCEEDED(hr) ? true : false;
	}
}


bool IGCommunicable::terminateClipboard(LPDATAOBJECT pIDataObject, int mode)
{
	bool bResult = false;
	if (mode == DATADIR_GET)
	{
		HRESULT hr = :: OleSetClipboard(pIDataObject);
		bResult = SUCCEEDED(hr) ? true : false;
	}
	pIDataObject->Release();
	return bResult;
}

bool IGCommunicable::copyToClipboard()
{
	bool bResult = false;
	LPDATAOBJECT pIDataObject = 0;
	if (initiateClipboard(&pIDataObject, DATADIR_GET))
    {
// fix bug 20030834 : Copy no longer copy a bitmap
// take the first hwnd available the best way should be to pass the hWnd as parameter
		HWND hWnd = 0;
		if (!dropTargetsList_.empty())
		{
			hWnd = dropTargetsList_.front()->getHWND();
		}
		bResult = get(pIDataObject, IGCommunicable::COPY_PASTE_ID, hWnd, 0, 0);

		terminateClipboard(pIDataObject, DATADIR_GET);
    }
	return bResult;
}

bool IGCommunicable::pasteFromClipboard()
{
	bool bResult = false;
	LPDATAOBJECT pIDataObject = 0;
	if (initiateClipboard(&pIDataObject, DATADIR_SET))
    {
		bResult = set(pIDataObject, IGCommunicable::COPY_PASTE_ID);
		terminateClipboard(pIDataObject, DATADIR_SET);
    }
	return bResult;
}

bool
IGCommunicable::endExport (LPDATAOBJECT lpDataObject) const
{
	return false;
}

#if 0
bool IGCommunicable::pasteSpecialFromClipboard(HWND root)
{
	bool bResult = false;
	LPDATAOBJECT pIDataObject = 0;
	if (initiateClipboard(&pIDataObject, DATADIR_SET))
    {
		std::list<Format*> liste_formats;
		getFormatAvailable(liste_formats, DATADIR_SET, IGCommunicable::COPY_PASTE_ID);
		Format* pFormat = pasteSpecialDialog(root, liste_formats, pIDataObject);
		if (!pFormat)
		{
			terminateClipboard(pIDataObject, DATADIR_SET);
		}
		else
		{
			bResult = set(pIDataObject, IGCommunicable::COPY_PASTE_ID, 0, 0, 0, pFormat->getKobraID());
			terminateClipboard(pIDataObject, DATADIR_SET);
		}
    }
	return bResult;
}

Format* IGCommunicable::pasteSpecialDialog(HWND root, std::list<Format*>& liste, LPDATAOBJECT pIDataObject) const
{
	OLEUIPASTESPECIALW oleUIPasteSpecial = {0};
	oleUIPasteSpecial.cbStruct = sizeof(oleUIPasteSpecial);
	oleUIPasteSpecial.dwFlags = PSF_STAYONCLIPBOARDCHANGE | PSF_NOREFRESHDATAOBJECT;

	// find parent HWND
	HWND hWndOwner = root;
	oleUIPasteSpecial.hWndOwner = hWndOwner;
	HWND hWndTop = ::GetParent (hWndOwner);

	oleUIPasteSpecial.lpszCaption = NULL; // "Paste Special" by default;

	std::vector<OLEUIPASTEENTRYW> formatEntries;

	OLEUIPASTEENTRYW oleUIPasteEntry;
	FORMATETC fmt;
	ULONG fetched;

	IEnumFORMATETC* pIEnumFormats;
	pIDataObject->EnumFormatEtc(DATADIR_GET, &pIEnumFormats);
	pIEnumFormats->Reset();
	while (pIEnumFormats->Next(1, &fmt, &fetched) == S_OK)
	{
		for(std::list<Format*>::iterator iter = liste.begin(); iter != liste.end(); ++iter)
		{
			if (fmt.cfFormat == (*iter)->getSystemID())
			{
				::ZeroMemory(&oleUIPasteEntry, sizeof(oleUIPasteEntry));
				oleUIPasteEntry.fmtetc = fmt;
				// Standard Format
				std::wstring sFormatName;
				std::wstring sResultText;
				if ((*iter)->getSystemID() == static_cast<UINT>((*iter)->getKobraID()))
				{
					sFormatName = sResultText = (*iter)->getName();
				}
				else
				{
					sFormatName = sResultText = (*iter)->getName();
				}
				oleUIPasteEntry.lpstrFormatName = new wchar_t[sFormatName.size() + 1];
				wcscpy_s( const_cast<wchar_t*>(oleUIPasteEntry.lpstrFormatName), sFormatName.size() + 1, sFormatName.c_str() );
				oleUIPasteEntry.lpstrResultText = new wchar_t[sResultText.size() + 1];
				wcscpy_s( const_cast<wchar_t*>(oleUIPasteEntry.lpstrResultText), sResultText.size() + 1, sResultText.c_str() );
				formatEntries.push_back(oleUIPasteEntry);
				++oleUIPasteSpecial.cPasteEntries;
				break;
			}
		}
	}
	pIEnumFormats->Release();

	oleUIPasteSpecial.arrPasteEntries = &formatEntries[0];

	if (::IsWindow(hWndTop))
	{
		::EnableWindow(hWndTop, FALSE);
	}
	int nResult = ::OleUIPasteSpecial(&oleUIPasteSpecial);
	if (::IsWindow(hWndTop))
	{
		::EnableWindow(hWndTop, TRUE);
	}

	std::vector<OLEUIPASTEENTRYW>::const_iterator iterOleUIPasteSpecial;
	for(iterOleUIPasteSpecial = formatEntries.begin(); iterOleUIPasteSpecial != formatEntries.end(); ++iterOleUIPasteSpecial)
	{
		delete[] (wchar_t*) (*iterOleUIPasteSpecial).lpstrFormatName;
		delete[] (wchar_t*) (*iterOleUIPasteSpecial).lpstrResultText;
	}
	if (nResult == OLEUI_OK)
	{
		iterOleUIPasteSpecial = formatEntries.begin();
		std::advance(iterOleUIPasteSpecial, oleUIPasteSpecial.nSelectedIndex);
		for(std::list<Format*>::iterator iter = liste.begin(); iter != liste.end(); ++iter)
		{
			if ((*iterOleUIPasteSpecial).fmtetc.cfFormat == (*iter)->getSystemID())
			{
				return (*iter);
			}
		}
	}
	return 0;
}
#endif

bool IGCommunicable::isClipboardFormatAvailable(int fid /*= Format::F_NONE*/) const
{
	std::list<FormatCommunication*>::const_iterator iter = communicationFormats_.begin();
	while(iter != communicationFormats_.end())
	{
		if (((fid == Format::F_NONE) || ((*iter)->getKobraID() == fid)) &&
			((*iter)->cid() & IGCommunicable::COPY_PASTE_ID) &&
			((*iter)->mode() & DATADIR_SET) &&
			 (*iter)->format()->isClipboardFormatAvailable())
		{
			return true;
		}
		++iter;
	}
	return false;
}

bool IGCommunicable::existClipboardFormat(int fid /*= Format::F_NONE*/) const
{
	return existExportFormat(fid, IGCommunicable::COPY_PASTE_ID);
}

bool IGCommunicable::existDragDropFormat(int fid /*= Format::F_NONE*/) const
{
	return existExportFormat(fid, IGCommunicable::DRAG_DROP_ID);
}

bool IGCommunicable::existExportFormat(int fid, int cid) const
{
	std::list<FormatCommunication*>::const_iterator iter = communicationFormats_.begin();
	while(iter != communicationFormats_.end())
	{
		if (((fid == Format::F_NONE) || ((*iter)->getKobraID() == fid)) &&
		    ((*iter)->cid() & cid) &&
			((*iter)->mode() & DATADIR_GET))
		{
			return true;
		}
		++iter;
	}
	return false;
}

class KO_CommunicableFake : public IGCommunicable
{
public :
	KO_CommunicableFake(std::ostrstream& vos, int fid)
		: IGCommunicable()
		, m_postream(&vos)
		, m_fid(fid)
	{
		registerFormat(fid, IGCommunicable::DRAG_DROP_ID | IGCommunicable::COPY_PASTE_ID, Format::MODE_OUT);
	}

	virtual int exportData(std::ostream& vos, int fid, int /*cid*/, int /*x*/, int /*y*/, HWND /*hwnd*/)
	{
		if (fid == m_fid)
		{
//			std::istream is(m_postream->rdbuf());
			vos << m_postream->rdbuf();
			return 1;
		}
		return 0;
	}

protected :
	std::ostrstream* m_postream;
	int		 m_fid;
};

// static
bool IGCommunicable::CopyToClipboard(std::ostrstream& vos, int fid)
{
	KO_CommunicableFake commFake(vos, fid);
	return commFake.copyToClipboard();
}

bool
IGCommunicable::importFormatAllowed(LPDATAOBJECT /*lpDataObject*/, int /*cid*/, HWND /*hWnd*/, int /*x*/, int /*y*/, int /*theFid*/) const
{
	return true;
}

STDMETHODIMP_(ULONG) IGCommunicable::AddRef()
{
	return ++m_refs;
}

STDMETHODIMP_(ULONG) IGCommunicable::Release()
{
	if(--m_refs == 0)
	{
		//And now we can delete this object
		delete this;
		return 0;
	}   
	return m_refs;
}

//----------------------------------------------------------------------
//  NAME        :IGDropSource
//
//  DESCRIPTION :IDropSource interface construction
//               Note that this IDropSource object aggregates an
//               OLE IDataObject.
//
//  PARAMETERS  : dataobject: Pointer on the Data Object to drag.
//
//  RETURNED VALUE:None
//----------------------------------------------------------------------
IGDropSource::IGDropSource(IGCommunicable* pKO_Communicable)
: m_pCommunicable(pKO_Communicable)
, m_refs(1)
{
	m_pCommunicable->AddRef();
}


//----------------------------------------------------------------------
//  NAME        :~IGDropSource
//
//  DESCRIPTION :IDropSource interface destruction.
//               We need to also delete the IDataObject which we
//               created for our IDropSource
//
//  RETURNED VALUE:None
//----------------------------------------------------------------------
IGDropSource::~IGDropSource()
{
	m_pCommunicable->Release();
}

//----------------------------------------------------------------------
//  NAME        :QueryInterface
//
//  DESCRIPTION :QueryInterface function for IDropSource interface.
//
//  RETURNED VALUE:See OLE documentation
//----------------------------------------------------------------------
STDMETHODIMP IGDropSource::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	if (ppvObj == 0)
	{
		return E_INVALIDARG;
	}
	*ppvObj = 0;
	if (::IsEqualIID(riid, IID_IUnknown))
	{
		*ppvObj = static_cast<IUnknown*>(this);
	}
	else if (::IsEqualIID(riid, IID_IDropSource))
	{
		*ppvObj = static_cast<IDropSource*>(this);
	}
	if (*ppvObj == 0)
	{
		return E_NOINTERFACE;
	}
	AddRef();
	return S_OK;
}


//----------------------------------------------------------------------
//  NAME        :AddRef
//
//  DESCRIPTION :See OLE documentation
//
//  RETURNED VALUE:See OLE documentation
//----------------------------------------------------------------------
STDMETHODIMP_(ULONG) IGDropSource::AddRef(void)
{
	return ++m_refs;
}

//----------------------------------------------------------------------
//  NAME        :Release
//
//  DESCRIPTION :See OLE documentation
//
//  RETURNED VALUE:See OLE documentation
//----------------------------------------------------------------------
STDMETHODIMP_(ULONG) IGDropSource::Release(void)
{
	if(--m_refs == 0)
	{
		//And now we can delete this object
		delete this;
		return 0;
	}   
	return m_refs;
}

//----------------------------------------------------------------------
//  NAME        :QueryContinueDrag
//
//  DESCRIPTION :Checks to see if the Drag operation should be
//               canceled, continued or a drop should occur.
//
//  RETURNED VALUE:See OLE documentaition
//----------------------------------------------------------------------
STDMETHODIMP IGDropSource::QueryContinueDrag (BOOL fEscapePressed, DWORD grfKeyState)
{
	if (fEscapePressed)
		return DRAGDROP_S_CANCEL;
	else
		if (!(grfKeyState & MK_LBUTTON))
			return DRAGDROP_S_DROP;
		else
			return S_OK;
}

//----------------------------------------------------------------------
//  NAME        :GiveFeedback
//
//  DESCRIPTION :Provides mouse cursor feedback during a drag
//               and drop. If you return DRAGDROP_S_USEDEFAULTCURSOR
//               OLE will use the appropriate default cursor for the
//               current drag drop operation
//
//  RETURNED VALUE:See OLE documentation
//----------------------------------------------------------------------
STDMETHODIMP IGDropSource::GiveFeedback(DWORD /*dwEffect*/ )
{
#if defined(DEBUG)&&defined(MYMSG)
	std::cout<<"IGDropSource::GiveFeedback() called."<<std::endl;
#endif
	return DRAGDROP_S_USEDEFAULTCURSORS;
}

// PB 1
// That should me more efficient (Memory & CPU) to check 
// the drop permission only when we enter in a new window (DragEnter)
//bool IGDropTarget::bAcceptDrop_ = false;

//----------------------------------------------------------------------
IGDropTarget::IGDropTarget(IGCommunicable* pKO_Communicable, HWND hWnd)
: m_pCommunicable(pKO_Communicable)
, m_hWnd(hWnd)
, m_refs(1)
, m_pDataObject(0)
{
}

//----------------------------------------------------------------------
IGDropTarget::~IGDropTarget()
{
	if (m_pDataObject)
	{
		m_pDataObject->Release();
		m_pDataObject = 0;
	}
}

//----------------------------------------------------------------------
STDMETHODIMP IGDropTarget::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
	if (ppvObj == 0)
	{
		return E_INVALIDARG;
	}
	*ppvObj = 0;
	if (::IsEqualIID(riid, IID_IUnknown))
	{
		*ppvObj = static_cast<IUnknown*>(this);
	}
	else if (::IsEqualIID(riid, IID_IDropTarget))
	{
		*ppvObj = static_cast<IDropTarget*>(this);
	}
	if (*ppvObj == 0)
	{
		return E_NOINTERFACE;
	}
	AddRef();
	return S_OK;
}

//----------------------------------------------------------------------
STDMETHODIMP_(ULONG) IGDropTarget::AddRef()
{
#if defined(DEBUG)&&defined(MYMSG)
	std::cout<<"IGDropTarget::AddRef() called."<<std::endl;
#endif
	return ++m_refs;
}

//----------------------------------------------------------------------
STDMETHODIMP_(ULONG) IGDropTarget::Release()
{
#if defined(DEBUG)&&defined(MYMSG)
	std::cout<<"IGDropTarget::Release() called"<<std::endl;
#endif
	if(--m_refs == 0)
	{
		//And now we can delete this object
		delete this;
		return 0;
	}  
	return m_refs;
}

//----------------------------------------------------------------------
bool IGDropTarget::matchFormat(LPDATAOBJECT pDataObject) const
{
	if( pDataObject == NULL )
		return false;

	if( m_pCommunicable == NULL )
		return false;

	std::list<Format*> liste_formats;
	int count = m_pCommunicable->getFormatAvailable(liste_formats, DATADIR_SET, IGCommunicable::DRAG_DROP_ID);
	if( count == 0 )
		return false;

	CComPtr<IEnumFORMATETC> spEnumFormats;
	HRESULT hr = pDataObject->EnumFormatEtc(DATADIR_GET, &spEnumFormats);
	if (FAILED(hr))
	{
		return false;
	}

	spEnumFormats->Reset();

	FORMATETC fmt;
	ULONG fetched;
	bool found = false;
	while (!found && (spEnumFormats->Next(1, &fmt, &fetched) == S_OK))
	{
		std::list<Format*>::const_iterator iter = liste_formats.begin();
		while(!found && (iter != liste_formats.end()))
		{
			Format* formatEtc = (*iter);
			// Check Public Formats
			found = (formatEtc->getSystemID() == fmt.cfFormat);
			++iter;
		}
	}
	return found;
}

//----------------------------------------------------------------------
STDMETHODIMP IGDropTarget::DragEnter(LPDATAOBJECT pDataObject, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)
{
#if defined(DEBUG)&&defined(MYMSG)
	std::cout<<"IGDropTarget::DragEnter() called."<<std::endl;
#endif
	if (pDataObject == NULL)
	{
		*pdwEffect = DROPEFFECT_NONE;
		return S_OK;
	}

	m_pDataObject = pDataObject;
	pDataObject->AddRef();

	if (!m_pCommunicable->acceptDropAction())
	{
		*pdwEffect = DROPEFFECT_NONE;
		return S_OK;
	}
	else if (!matchFormat(pDataObject))
	{
		*pdwEffect = DROPEFFECT_NONE;
		return S_OK;
	}


	int xcoord = pt.x;
	int ycoord = pt.y;

	if (::IsWindow(m_hWnd)) 
	{
		RECT rect;
		::GetWindowRect(m_hWnd, (LPRECT) &rect);
		xcoord -= rect.left;
		ycoord -= rect.top;
	}

	return m_pCommunicable->setCursorEffect(grfKeyState,xcoord,ycoord,(int*)pdwEffect);
}

//----------------------------------------------------------------------
STDMETHODIMP IGDropTarget::DragOver(DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)
{
#if defined(DEBUG)&&defined(MYMSG)
	std::cout<<"IGDropTarget::DragOver() called."<<std::endl;
#endif
	// PB 1
	/*
	if (!bAcceptDrop_)
	{
	*pdwEffect = DROPEFFECT_NONE;
	}
	*/
	if (!m_pCommunicable->acceptDropAction())
	{
		*pdwEffect = DROPEFFECT_NONE;
		return S_OK;
	} 
	else  if (!matchFormat(m_pDataObject))
	{  
		*pdwEffect = DROPEFFECT_NONE;
		return S_OK;
	}		

	int xcoord = pt.x;
	int ycoord = pt.y;

	if (::IsWindow(m_hWnd)) 
	{
		RECT rect;
		::GetWindowRect(m_hWnd, (LPRECT) &rect);
		xcoord -= rect.left;
		ycoord -= rect.top;
	}

	return m_pCommunicable->setCursorEffect(grfKeyState,xcoord,ycoord,(int*)pdwEffect);
}

//----------------------------------------------------------------------
STDMETHODIMP IGDropTarget::DragLeave()
{
#if defined(DEBUG)&&defined(MYMSG)
	std::cout<<"IGDropTarget::DragLeave() called."<<std::endl;
#endif
	// PB 1
	if (m_pDataObject)
	{
		m_pDataObject->Release();
		m_pDataObject = 0;
	}
	return S_OK;
}

//----------------------------------------------------------------------
STDMETHODIMP IGDropTarget::Drop(LPDATAOBJECT pDataObject, DWORD /*grfKeyState*/, POINTL pt, LPDWORD /*pdwEffect*/)
{
	if (m_pDataObject)
	{
		m_pDataObject->Release();
		m_pDataObject = 0;
	}
	// Prevent from dropping inside the same object
	if (m_pCommunicable->isDragInitiator() && !m_pCommunicable->getAcceptInnerDragDrop())
	{
		return 0;
	}
	// Check to see if the object accept a drop
	if (!m_pCommunicable->acceptDropAction())
	{
		return 0;
	}

	int xcoord = pt.x;
	int ycoord = pt.y;

	if (::IsWindow(m_hWnd)) 
	{
		RECT rect;
		::GetWindowRect(m_hWnd, (LPRECT) &rect);
		xcoord -= rect.left;
		ycoord -= rect.top;
	}

	m_pCommunicable->set(pDataObject, IGCommunicable::DRAG_DROP_ID, m_hWnd, xcoord, ycoord);

	return S_OK;
}