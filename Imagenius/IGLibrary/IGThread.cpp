#include "StdAfx.h"
#include "ximage.h"
#include "IGThread.h"
#include "IGException.h"
#include "IGLog.h"
#include <process.h>
#include <winuser.h>
#include <dbghelp.h>

using namespace IGLibrary;

#define REQUESTACCESS_ASSERTE(a)	 //_ASSERTE(a)
//#define DUMPTRACE
#define STARTDUMP(file)\
{	char pcTempPath [MAX_PATH];\
	::GetTempPathA (MAX_PATH, pcTempPath);\
	std::string sDumpPath (pcTempPath);\
	sDumpPath += "\\imagenius_trace.txt";\
	(file) = ::fopen (sDumpPath.c_str(), "a");\
	SYSTEMTIME st;\
	::GetSystemTime (&st);\
	::fwprintf (pFile, L"%d/%d/%d %dh %dmn %ds %3dms - ", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#define ENDDUMP(file)\
	::fclose (file);}

int CreateMiniDump( EXCEPTION_POINTERS* pep ) 
{
  // Open the file 
	SYSTEMTIME st;
	::GetSystemTime (&st);
	std::wstring wsDumpPath (L"MiniDump-");
	wchar_t pwTime [MAX_PATH];
	::swprintf_s (pwTime, L"%d-%d-%d_%dh_%dmn_%ds_%3dms", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	wsDumpPath += pwTime;
	wsDumpPath += L".dmp";
	HANDLE hFile = CreateFile( wsDumpPath.c_str(), GENERIC_READ | GENERIC_WRITE, 
							0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ); 

	IGLog log;
	if( ( hFile != NULL ) && ( hFile != INVALID_HANDLE_VALUE ) ) 
	{
		// Create the minidump 
		MINIDUMP_EXCEPTION_INFORMATION mdei; 
		mdei.ThreadId           = GetCurrentThreadId(); 
		mdei.ExceptionPointers  = pep; 
		mdei.ClientPointers     = FALSE; 
		MINIDUMP_TYPE mdt       = MiniDumpNormal; 

		BOOL rv = MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), 
		hFile, mdt, (pep != 0) ? &mdei : 0, 0, 0 ); 
		
		std::string excStr ("Unknown exception caught in IGThread processing - ");		
		if( !rv ) 
			excStr += "MiniDumpWriteDump failed"; 
		else 
			excStr += "Minidump created"; 
		log.Add (excStr.c_str());

		// Close the file 
		CloseHandle( hFile ); 
	}
	else 
	{
		log.Add ("Create MiniDump file failed"); 
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

UINT IGThread::UM_WAKEUP = RegisterWindowMessageW (L"WakeUp");

IGThreadMessage::IGThreadMessage(int nParentThreadId, HWND hProgress, HWND hParent)	: m_nMsg (0)
																					, m_nMagicKey (IGTHREADDATA_MAGICKEY)
																					, m_hProgress (hProgress)
																					, m_hParent (hParent)
																					, m_nParentThreadId (nParentThreadId)
{
}

IGThreadMessage::~IGThreadMessage ()
{
}

IGThread::IGThread(void)	: m_hThread (NULL)
							, m_hStartEvent (NULL)
							, m_hWaitEvent (NULL)
							, m_hExitEvent (NULL)
							, m_bExitThread (false)
							, m_bIsWorking (false)
							, m_bSynchroneRequest (false)
							, m_nSynchroneRequestMessage (0)
							, m_dwCreatorThreadId (0)
							, m_pnThreadId (new UINT)
							, m_pdwWorkingThreadId (new DWORD)
							, m_pbThreadStarted (new BOOL)
{
	// initialize critical section
	::InitializeCriticalSection (&m_CSThread);
	// create thread event handles
	m_hStartEvent = ::CreateEventW(0, TRUE, FALSE, L"");
	m_hWaitEvent = ::CreateEventW(0, TRUE, FALSE, L"");
	m_hExitEvent = ::CreateEventW(0, TRUE, FALSE, L"");
	// Initialize thread info
	m_dwCreatorThreadId = ::GetCurrentThreadId ();
	*m_pdwWorkingThreadId = m_dwCreatorThreadId;
	*m_pbThreadStarted = FALSE;
	*m_pnThreadId = 0;
}

IGThread::~IGThread(void)
{	
	if (m_hThread)
	{
		_ASSERTE (FALSE && L"Error: destroying an object whose thread is running");
		Exit();
	}
	// delete multithread state volatile vars
	delete m_pbThreadStarted;
	delete m_pdwWorkingThreadId;
	delete m_pnThreadId;
	// delete critical section
	::DeleteCriticalSection (&m_CSThread);
	// release thread event handles
	::CloseHandle (m_hStartEvent);
	::CloseHandle (m_hWaitEvent);
	::CloseHandle (m_hExitEvent);
}

bool IGThread::Start()
{
	if (m_hThread)
	{
		_ASSERTE (FALSE && L"Warning: thread is already started");
		return true;
	}

	// Create the thread
	UINT nThreadId;
	m_hThread = (HANDLE)::_beginthreadex(NULL, 0, IGThread::staticInternalThreadProc, this, 0, &nThreadId);
	*m_pnThreadId = nThreadId;
	
	if (!m_hThread)
	{
		_ASSERTE (FALSE && L"Error: thread failed to start");
		return false;
	}

	if (::WaitForSingleObject (m_hStartEvent, IGTHREAD_TIMEOUT) != WAIT_OBJECT_0)
	{
		_ASSERTE (FALSE && L"Error: thread initialization time out");
		::CloseHandle (m_hThread);
		m_hThread = NULL;
		return false;
	}	

	::InterlockedExchange ((volatile LONG*)m_pbThreadStarted, TRUE);
	return true;
}

bool IGThread::Exit()
{
	if (!m_hThread)
	{
		_ASSERTE (FALSE && L"Warning: stopping a thread that is not started");
		return true;
	}
	
	// enter critical section
	{
		SmartLock lock(&m_CSThread);
		m_bExitThread = true;
	}

	bool bRet = true;
	// shake the thread
	WakeUp();
	if (::WaitForSingleObject (m_hExitEvent, IGTHREAD_TIMEOUT) != WAIT_OBJECT_0)
	{
		_ASSERTE (FALSE && L"Error: exit thread time out");
		bRet = false;
	}
	
	::CloseHandle (m_hThread);
	m_hThread = NULL;
	::InterlockedExchange ((volatile LONG*)m_pbThreadStarted, FALSE);
	return bRet;
}

bool IGThread::SendMsg (const IGThreadMessage *pMessage)
{
	return processMsg (pMessage, true);
}

bool IGThread::PostMsg (const IGThreadMessage *pMessage)
{
	return processMsg (pMessage, false);
}

bool IGThread::processMsg (const IGThreadMessage *pMessage, bool bSynchrone)
{
#ifdef DUMPTRACE
	FILE *pFile;
	STARTDUMP(pFile)
	::fwprintf (pFile, L"Sending image processing message %d to thread %d %s\n", 
				pMessage ? pMessage->GetMsgId() : -1, *m_pnThreadId,
				IsStarted() ? L"started" : L"stopped");
	ENDDUMP(pFile)
#endif
	if (pMessage && IsStarted())
	{
		if (bSynchrone)
		{
			{
				SmartLock lock(&m_CSThread);
				m_bSynchroneRequest = true;
				m_nSynchroneRequestMessage = pMessage->GetMsgId();
			}
			::ResetEvent (m_hWaitEvent);
		}
		BOOL bMessagePosted = FALSE;
		UINT nNbTry = 100;
		while (!bMessagePosted && (nNbTry-- > 0))
		{
			bMessagePosted = ::PostThreadMessageW (*m_pnThreadId, pMessage->GetMsgId(), (WPARAM)pMessage, 0);
			if (!bMessagePosted)
			{
				::Sleep (25);
#ifdef DUMPTRACE
				STARTDUMP(pFile)
				::fwprintf (pFile, L"Sending message %d to thread %d failed - Retrying...\n", 
							pMessage->GetMsgId(), *m_pnThreadId);
				ENDDUMP(pFile)
#endif
			}
		}
		_ASSERTE (bMessagePosted && L"An image processing could not be sent to frame image processing thread");
		if (bSynchrone)
		{
			if (::WaitForSingleObject (m_hWaitEvent, IGTHREAD_TIMEOUT) != WAIT_OBJECT_0)
			{
				_ASSERTE (FALSE && L"Error: synchrone request time out");
				return false;
			}
		}
		return true;
	}
	return false;
}

unsigned __stdcall IGThread::internalThreadProc()
{
	// Initialize thread object
	InitProc ();
	::SetEvent (m_hStartEvent);

	MSG msg;
	for( ;; )
	{
		// Main message loop:
		if (::GetMessage (&msg, NULL, 0, 0))
		{
			__try
			{
				// The current thread is now processing a message
				m_bIsWorking = true;

	#ifdef DUMPTRACE
				FILE *pFile;
				STARTDUMP(pFile)
				::fwprintf (pFile, L"Thread id %d - Processing message id %d\n", *m_pnThreadId, msg.message);
				ENDDUMP(pFile)
	#endif
				if (msg.message >= UM_IGROOT)	// Process Imagenius user messages only
				{
					// First check if the message is an IGThreadMessage
					IGThreadMessage *pMessage = static_cast <IGThreadMessage *> ((void *)msg.wParam);
					if (pMessage	&& pMessage->IsValid()
									&& (msg.message == pMessage->GetMsgId()))
					{
						// Switching memory handler to current thread
						SetWorkingThreadId (*m_pnThreadId);
	#ifdef DUMPTRACE
						STARTDUMP(pFile)
						::fwprintf (pFile, L"%s %d %s %d\n", L"Starting image processing id",
									pMessage->GetMsgId(), L"in thread", *m_pnThreadId);
						ENDDUMP(pFile)
	#endif
						// Process thread message
						ThreadProc (pMessage);

	#ifdef DUMPTRACE
						STARTDUMP(pFile)
						::fwprintf (pFile, L"%s %d %s %d\n", L"Ending image processing id",
									pMessage->GetMsgId(), L"in thread", *m_pnThreadId);
						ENDDUMP(pFile)
	#endif
					}
					else
					{
						// Unknown message
						::TranslateMessage (&msg);
						::DispatchMessage (&msg);
						//_ASSERTE (0 && L"Unknow image processing message");
					}
				}
				else
				{
					// window message
					::TranslateMessage (&msg);
					::DispatchMessage (&msg);
				}

				// Message processing has ended
				m_bIsWorking = false;

	#ifdef DUMPTRACE
				STARTDUMP(pFile)
				::fwprintf (pFile, L"Thread id %d - Idle\n", *m_pnThreadId);
				ENDDUMP(pFile)
	#endif
				if (m_bSynchroneRequest && msg.message == m_nSynchroneRequestMessage)
				{
					m_bSynchroneRequest = false;
					m_nSynchroneRequestMessage = 0;
					::SetEvent (m_hWaitEvent);				
				}
			}
			__except( CreateMiniDump( GetExceptionInformation() ) ) 
			{
				// critical error, exit the process
				exit(0);
			}
		}

		// Check if a thread exit has been requested
		if (m_bExitThread == true)
		{
			m_bExitThread = false;
			break;
		}		
	}

	TermProc ();
	SetWorkingThreadId (0);
	m_dwCreatorThreadId = 0;

	::SetEvent (m_hExitEvent);
	return 0;
}

unsigned __stdcall IGThread::staticInternalThreadProc(void* p)
{
	return reinterpret_cast<IGThread*>(p)->internalThreadProc();
}

bool IGThread::WakeUp() const
{
	if (m_hThread)
	{
		::PostThreadMessageW (*m_pnThreadId, UM_WAKEUP, 0, 0);
		return true;
	}
	return false;
}

void IGThread::IsWorking (bool *pbIsWorking)
{
	{
		SmartLock lock(&m_CSThread);
		*pbIsWorking = m_bIsWorking;
	}
}

void IGThread::GetWorkingThreadId (LONG &nThreadId)
{
	nThreadId = m_pdwWorkingThreadId ? *m_pdwWorkingThreadId : -1;
	_ASSERTE ((nThreadId > 0) && L"Invalid thread id");
}

void IGThread::SetWorkingThreadId (LONG nThreadId)
{
	::InterlockedExchange ((volatile LONG*)m_pdwWorkingThreadId, nThreadId);
}

BOOL IGThread::IsStarted () const
{
	return *m_pbThreadStarted;
}

bool IGThread::RequestAccess()
{
	DWORD dwCurrentThreadId = 0;
	LONG nWorkingThreadId = -1;
	GetWorkingThreadId (nWorkingThreadId);
	dwCurrentThreadId = ::GetCurrentThreadId();

	REQUESTACCESS_ASSERTE ((dwCurrentThreadId == nWorkingThreadId) && L"IGThread access denied");
	return (dwCurrentThreadId == nWorkingThreadId);
}

void IGThread::GrantAccess()
{
	// Switching memory handler back to thread creator
	SetWorkingThreadId (m_dwCreatorThreadId);
}