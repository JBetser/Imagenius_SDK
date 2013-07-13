#pragma once
#include <windows.h>
#include <string>

#define IGTHREAD_TIMEOUT		10000	// 10s timeout to avoid freezing
#define IGTHREADDATA_MAGICKEY	0xABCD

namespace IGLibrary
{
// multithreading message object to post message to imagenius threads
// once a IGThreadMessage is posted, NEVER ACCESS IT OR DELETE IT
class IGThreadMessage
{
	// The class that processes the message
	friend class IGImageProcessing;

public:
	IGThreadMessage (int nParentThreadId = 0, HWND hProgress = NULL, HWND hParent = NULL);
	virtual ~IGThreadMessage ();

	template <class IGThreadChildMessage>
	void operator() (IGThreadChildMessage *pChildMessage) const
	{
		_ASSERTE (GetThisMessage <IGThreadChildMessage> () && L"Invalid IGThread message");
		return GetThisMessage <IGThreadChildMessage> ();
	}

	virtual bool IsValid () = 0;
	virtual UINT GetMsgId () const = 0;
	inline HWND GetParentWnd() const {return m_hParent;}
	inline HWND GetProgressBar() const {return m_hProgress;}
	inline int GetParentThreadId() const {return m_nParentThreadId;}

protected:
	// This method shall be called by each ThreadProc implementation
	// to obtain the expected IGThreadMessage object
	template <class IGThreadChildMessage> 
	IGThreadChildMessage *GetThisMessage ()
	{
		if (m_nMagicKey != IGTHREADDATA_MAGICKEY)
			return NULL;
		return dynamic_cast <IGThreadChildMessage *> (this);
	}

	template <class IGThreadChildMessage>
	bool InternalIsValid ()
	{
		if (GetThisMessage <IGThreadChildMessage> ())
			return true;
		_ASSERTE (0 && L"Invalid IGThread message");
		return false;
	}

	HWND m_hParent;
	int m_nParentThreadId;
	HWND m_hProgress;

private:
	UINT m_nMagicKey;
	UINT m_nMsg;
};

class IGThread
{
public:
	static UINT UM_WAKEUP;

	IGThread(void);
	virtual ~IGThread(void);

// *******************************************
// THREAD OWNER METHODS
	// Start the thread
	virtual bool Start();

	// Destroy the thread
	virtual bool Exit();

	// Send a synchronous message to the thread
	virtual bool SendMsg (const IGThreadMessage *pMessage);

	// Post an asynchronous message to the thread
	virtual bool PostMsg (const IGThreadMessage *pMessage);

	// A fake message to make the thread processing
	virtual bool WakeUp () const;	
// *******************************************

// *******************************************
// EXTERNAL THREAD METHOD
	// Check if the thread is currently processing a message
	void IsWorking (bool *pbIsWorking);
// *******************************************

// *******************************************
// THREAD-SAFE METHODS
	// Get the id of the thread which is currently handling the object memory
	// It can be either the thread creator or the created thread
	void GetWorkingThreadId (LONG &nThreadId);
	void SetWorkingThreadId (LONG nThreadId);

	// check if current thread has the right to access the object memory
	bool RequestAccess();

	// call this function when access to protected resource can be granted
	void GrantAccess();

	// return true if current thread is started
	BOOL IsStarted () const;
// *******************************************

protected:
	// Abstract methods
	virtual void InitProc () {};
	virtual void TermProc () {};
	virtual void ThreadProc (IGThreadMessage *pMessage) = 0;

	// critical section can be accessed by derived classes
	CRITICAL_SECTION		m_CSThread;

private:
	// Thread
	static unsigned __stdcall staticInternalThreadProc(void* p);
	virtual unsigned __stdcall internalThreadProc();

// THREAD OWNER METHOD
	bool processMsg (const IGThreadMessage *pMessage, bool bSynchrone);
////////////////////////

	UINT					*m_pnThreadId;	// the created thread id
	DWORD					*m_pdwWorkingThreadId;	
	BOOL					*m_pbThreadStarted;
	HANDLE					m_hThread;
	HANDLE					m_hStartEvent;
	HANDLE					m_hWaitEvent;
	HANDLE					m_hExitEvent;	
	bool					m_bExitThread;
	bool					m_bSynchroneRequest;
	bool					m_bIsWorking;
	DWORD					m_dwCreatorThreadId;
	UINT					m_nSynchroneRequestMessage;
};

class SmartLock
{
public:

	SmartLock(LPCRITICAL_SECTION sect)
	:m_pLock(sect)
	{
		::EnterCriticalSection(m_pLock);
	};

	~SmartLock()
	{
		::LeaveCriticalSection( m_pLock );
	};

private:
	LPCRITICAL_SECTION m_pLock;
};

}